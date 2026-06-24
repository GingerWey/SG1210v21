//-----------------------------------------------------------------------------
/*
 File        : GUIHEAP.cpp
 Version     : V1.10
 By          : 银网科技

 Description :管理RAM-Heap的对象

 Date       : 2023.12.26

 v2.0
   升级到10.3.1算法
*/
//-----------------------------------------------------------------------------
#include "GUIHEAP.h"

#include "DevDebug.h"

#include <stdlib.h>
//=============================================================================
// 全局宏
//-----------------------------------------------------------------------------
// 0x4000 余  0x2470
#define  TOTAL_GUIHEAP_SIZE   0x01000     // 8KB
//-----------------------------------------------------------------------------
/* Block sizes must not get too small. */
#define GUIHEAP_MINIMUM_BLOCK_SIZE  ( (size_t)( gHeapStructSize << 1 ) )

/* Assumes 8bit bytes! */
#define GUIHEAP_BITS_PER_BYTE    ( (size_t)8 )

//=============================================================================
// 局部数据结构 
//-----------------------------------------------------------------------------
/* Define the linked list structure.  This is used to link free blocks in order
  of their memory address. */
typedef struct G_BLOCK_LINK
{
  struct G_BLOCK_LINK *pxNextFreeBlock;  /*<< The next free block in the list. */
  size_t xBlockSize;            /*<< The size of the free block. */
} GUIBlockLink_t;

//=============================================================================
// 局布数据 
//-----------------------------------------------------------------------------

/* Allocate the memory for the heap. */
static uint8_t ucGUIHEAP[ TOTAL_GUIHEAP_SIZE ];

/* The size of the structure placed at the beginning of each allocated memory
  block must by correctly byte aligned. */
static const size_t gHeapStructSize  = 
           ( sizeof(GUIBlockLink_t) + ( (size_t)(portBYTE_ALIGNMENT - 1) ) ) & 
           ~( (size_t)portBYTE_ALIGNMENT_MASK );

/* Create a couple of list links to mark the start and end of the list. */
static GUIBlockLink_t gStart, *GUIHEAP_pblEnd = nullptr;

/* Keeps track of the number of calls to allocate and free memory as well as the
  number of free bytes remaining, but says nothing about fragmentation. */
static size_t gGUIFreeBytesRemaining = 0U;
static size_t gGUIMinimumEverFreeBytesRemaining = 0U;
static size_t gGUINumberOfSuccessfulAllocations = 0;
static size_t gGUINumberOfSuccessfulFrees = 0;

/* Gets set to the top bit of an size_t type.  When this bit in the xBlockSize
  member of an GUIBlockLink_t structure is set then the block belongs to the
  application.  When the bit is free the block is still part of the free heap
  space. */
static size_t gGUIBlockAllocatedBit = 0;

//=============================================================================
// 内部方法申明
//-----------------------------------------------------------------------------
/*
   Inserts a block of memory that is being freed into the correct position in
   the list of free memory blocks.  The block being freed will be merged with
   the block in front it and/or the block behind it if the memory blocks are
   adjacent to each other.
*/
static void GUIHEAP_InsertBlockIntoFreeList ( GUIBlockLink_t *pxBlockToInsert );

//=============================================================================
// 全局方法
//-----------------------------------------------------------------------------
void *GUIHEAP_Malloc ( size_t xWantedSize )
{
  GUIBlockLink_t *pxBlock, *pxPreviousBlock, *pxNewBlockLink;
  void *pvReturn = nullptr;

  DEV_ASSERT( (0 == xWantedSize), GFC_ErrParam );

  vTaskSuspendAll();
    {
    /* If this is the first call to malloc then the heap will require
      initialisation to setup the list of free blocks. */
    if( GUIHEAP_pblEnd == nullptr )
      {
      GUIHEAP_Init();
      }
    else
      {
      mtCOVERAGE_TEST_MARKER();
      }

    /* Check the requested block size is not so large that the top bit is
      set.  The top bit of the block size member of the GUIBlockLink_t structure
      is used to determine who owns the block - the application or the
      kernel, so it must be free. */
    if( ( xWantedSize & gGUIBlockAllocatedBit ) == 0 )
      {
      /* The wanted size is increased so it can contain a GUIBlockLink_t
        structure in addition to the requested amount of bytes. */
      if( xWantedSize > 0 )
        {
        xWantedSize += gHeapStructSize;

        /* Ensure that blocks are always aligned to the required number
          of bytes. */
        if( ( xWantedSize & portBYTE_ALIGNMENT_MASK ) != 0x00 )
          {
          /* Byte alignment required. */
          xWantedSize += ( portBYTE_ALIGNMENT - (xWantedSize & portBYTE_ALIGNMENT_MASK) );
          configASSERT( ( xWantedSize & portBYTE_ALIGNMENT_MASK ) == 0 );
          }
        else
          {
          mtCOVERAGE_TEST_MARKER();
          }
        }
      else
        {
        mtCOVERAGE_TEST_MARKER();
        }

      if( (xWantedSize > 0) && (xWantedSize <= gGUIFreeBytesRemaining)  )
        {
        /* Traverse the list from the start  (lowest address) block until
          one  of adequate size is found. */
        pxPreviousBlock = &gStart;
        pxBlock = gStart.pxNextFreeBlock;
        while ( ( pxBlock->xBlockSize < xWantedSize ) &&
                ( pxBlock->pxNextFreeBlock != nullptr ) )
          {
          pxPreviousBlock = pxBlock;
          pxBlock = pxBlock->pxNextFreeBlock;
          }

        /* If the end marker was reached then a block of adequate size
          was  not found. */
        if( pxBlock != GUIHEAP_pblEnd )
          {
          /* Return the memory space pointed to - jumping over the
            GUIBlockLink_t structure at its start. */
          pvReturn = (void*)( ( (uint8_t*)pxPreviousBlock->pxNextFreeBlock ) +
                              gHeapStructSize );

          /* This block is being returned for use so must be taken out
            of the list of free blocks. */
          pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;

          /* If the block is larger than required it can be split into
            two. */
          if( ( pxBlock->xBlockSize - xWantedSize ) > GUIHEAP_MINIMUM_BLOCK_SIZE )
            {
            /* This block is to be split into two.  Create a new
              block following the number of bytes requested. The void
              cast is used to prevent byte alignment warnings from the
              compiler. */
            pxNewBlockLink = (GUIBlockLink_t*)( ( (uint8_t*) pxBlock ) + xWantedSize );
            configASSERT( (((size_t)pxNewBlockLink) & portBYTE_ALIGNMENT_MASK) == 0 );

            /* Calculate the sizes of two blocks split from the single block. */
            pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;
            pxBlock->xBlockSize = xWantedSize;

            /* Insert the new block into the list of free blocks. */
            GUIHEAP_InsertBlockIntoFreeList ( pxNewBlockLink );
            }
          else
            {
            mtCOVERAGE_TEST_MARKER();
            }

          gGUIFreeBytesRemaining -= pxBlock->xBlockSize;

          if( gGUIFreeBytesRemaining < gGUIMinimumEverFreeBytesRemaining )
            {
            gGUIMinimumEverFreeBytesRemaining = gGUIFreeBytesRemaining;
            }
          else
            {
            mtCOVERAGE_TEST_MARKER();
            }

          /* The block is being returned - it is allocated and owned
            by the application and has no "next" block. */
          pxBlock->xBlockSize |= gGUIBlockAllocatedBit;
          pxBlock->pxNextFreeBlock = nullptr;
          gGUINumberOfSuccessfulAllocations++;
          }
        else
          {
          mtCOVERAGE_TEST_MARKER();
          }
        }
      else
        {
        mtCOVERAGE_TEST_MARKER();
        }
      }
    else
      {
      mtCOVERAGE_TEST_MARKER();
      }

    traceMALLOC ( pvReturn, xWantedSize );
    }
  (void)xTaskResumeAll();

#if( configUSE_MALLOC_FAILED_HOOK == 1 )
    {
    if( pvReturn == nullptr )
      {
      extern void vApplicationMallocFailedHook(void);
      vApplicationMallocFailedHook();
      }
    else
      {
      mtCOVERAGE_TEST_MARKER();
      }
    }
#endif

  DEV_ASSERT( (nullptr == pvReturn), GFC_OutOfMem );

  configASSERT ((((size_t)pvReturn ) & (size_t)portBYTE_ALIGNMENT_MASK ) == 0 );

  return pvReturn;
}
//-----------------------------------------------------------------------------

void GUIHEAP_Free ( void *pv )
{
  
  uint8_t *puc = (uint8_t*) pv;
  GUIBlockLink_t *pxLink;

  if( pv != nullptr )
    {
    /* The memory being freed will have an GUIBlockLink_t structure immediately
      before it. */
    puc -= gHeapStructSize;

    /* This casting is to keep the compiler from issuing warnings. */
    pxLink = (GUIBlockLink_t*)puc;

    /* Check the block is actually allocated. */
    configASSERT ( ( pxLink->xBlockSize & gGUIBlockAllocatedBit ) != 0 );
    configASSERT ( pxLink->pxNextFreeBlock == nullptr );

    if( ( pxLink->xBlockSize & gGUIBlockAllocatedBit ) != 0 )
      {
      if( pxLink->pxNextFreeBlock == nullptr )
        {
        /* The block is being returned to the heap - it is no longer allocated. */
        pxLink->xBlockSize &= ~gGUIBlockAllocatedBit;

        vTaskSuspendAll();
          {
          /* Add this block to the list of free blocks. */
          gGUIFreeBytesRemaining += pxLink->xBlockSize;
          traceFREE ( pv, pxLink->xBlockSize );
          GUIHEAP_InsertBlockIntoFreeList ( ( (GUIBlockLink_t*)pxLink ) );
          gGUINumberOfSuccessfulFrees++;
          }
        (void)xTaskResumeAll();
        }
      else
        {
        mtCOVERAGE_TEST_MARKER();
        }
      }
    else
      {
      mtCOVERAGE_TEST_MARKER();
      }
    }
  else
    DEV_FAULT( GFC_OutOfMem );
}
//=============================================================================
// 内部方法
//-----------------------------------------------------------------------------
size_t GUIHEAP_GetFreeHeapSize ( void )
{
  return gGUIFreeBytesRemaining;
}
//-----------------------------------------------------------------------------

size_t GUIHEAP_MinimumEverFreeHeapSize ( void )
{
  return gGUIMinimumEverFreeBytesRemaining;
}
//-----------------------------------------------------------------------------

static void _InitialiseBlocks ( void )
{
  /* This just exists to keep the linker quiet. */
}
//-----------------------------------------------------------------------------

void GUIHEAP_Init ( void )
{
  GUIBlockLink_t *pxFirstFreeBlock;
  uint8_t *pucAlignedHeap;
  size_t uxAddress;
  size_t xTotalHeapSize = TOTAL_GUIHEAP_SIZE;

  /* Ensure the heap starts on a correctly aligned boundary. */
  uxAddress = (size_t)ucGUIHEAP;

  if( ( uxAddress & portBYTE_ALIGNMENT_MASK ) != 0 )
    {
    uxAddress +=  ( portBYTE_ALIGNMENT - 1 );
    uxAddress &= ~( (size_t)portBYTE_ALIGNMENT_MASK );
    xTotalHeapSize -= uxAddress - (size_t)ucGUIHEAP;
    }

  pucAlignedHeap = (uint8_t*) uxAddress;

  /* gStart is used to hold a pointer to the first item in the list of free
    blocks.  The void cast is used to prevent compiler warnings. */
  gStart.pxNextFreeBlock = (struct G_BLOCK_LINK*)pucAlignedHeap;
  gStart.xBlockSize = (size_t)0;

  /* GUIHEAP_pblEnd is used to mark the end of the list of free blocks and is inserted
    at the end of the heap space. */
  uxAddress  = ( (size_t)pucAlignedHeap ) + xTotalHeapSize;
  uxAddress -= gHeapStructSize;
  uxAddress &= ~( (size_t)portBYTE_ALIGNMENT_MASK );
  GUIHEAP_pblEnd = (GUIBlockLink_t*)uxAddress;
  GUIHEAP_pblEnd->xBlockSize = 0;
  GUIHEAP_pblEnd->pxNextFreeBlock = nullptr;

  /* To start with there is a single free block that is sized to take up the
    entire heap space, minus the space taken by GUIHEAP_pblEnd. */
  pxFirstFreeBlock = (GUIBlockLink_t*)pucAlignedHeap;
  pxFirstFreeBlock->xBlockSize = uxAddress - (size_t)pxFirstFreeBlock;
  pxFirstFreeBlock->pxNextFreeBlock = GUIHEAP_pblEnd;

  /* Only one block exists - and it covers the entire usable heap space. */
  gGUIMinimumEverFreeBytesRemaining = pxFirstFreeBlock->xBlockSize;
  gGUIFreeBytesRemaining = pxFirstFreeBlock->xBlockSize;

  /* Work out the position of the top bit in a size_t variable. */
  gGUIBlockAllocatedBit = ((size_t)1) << ((sizeof(size_t) * GUIHEAP_BITS_PER_BYTE) - 1);
}
//-----------------------------------------------------------------------------

static void GUIHEAP_InsertBlockIntoFreeList ( GUIBlockLink_t *pxBlockToInsert )
{
  GUIBlockLink_t *pxIterator;
  uint8_t *puc;

  /* Iterate through the list until a block is found that has a higher address
    than the block being inserted. */
  for ( pxIterator = &gStart; pxIterator->pxNextFreeBlock < pxBlockToInsert;
        pxIterator = pxIterator->pxNextFreeBlock )
    {
    /* Nothing to do here, just iterate to the right position. */
    }

  /* Do the block being inserted, and the block it is being inserted after
    make a contiguous block of memory? */
  puc = (uint8_t*) pxIterator;
  if( ( puc + pxIterator->xBlockSize ) == (uint8_t*) pxBlockToInsert )
    {
    pxIterator->xBlockSize += pxBlockToInsert->xBlockSize;
    pxBlockToInsert = pxIterator;
    }
  else
    {
    mtCOVERAGE_TEST_MARKER();
    }

  /* Do the block being inserted, and the block it is being inserted before
    make a contiguous block of memory? */
  puc = (uint8_t*) pxBlockToInsert;
  if( (puc + pxBlockToInsert->xBlockSize) == (uint8_t*)pxIterator->pxNextFreeBlock )
    {
    if( pxIterator->pxNextFreeBlock != GUIHEAP_pblEnd )
      {
      /* Form one big block from the two blocks. */
      pxBlockToInsert->xBlockSize += pxIterator->pxNextFreeBlock->xBlockSize;
      pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock->pxNextFreeBlock;
      }
    else
      {
      pxBlockToInsert->pxNextFreeBlock = GUIHEAP_pblEnd;
      }
    }
  else
    {
    pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;
    }

  /* If the block being inserted plugged a gab, so was merged with the block
    before and the block after, then it's pxNextFreeBlock pointer will have
    already been set, and should not be set here as that would make it point
    to itself. */
  if( pxIterator != pxBlockToInsert )
    {
    pxIterator->pxNextFreeBlock = pxBlockToInsert;
    }
  else
    {
    mtCOVERAGE_TEST_MARKER();
    }
}
//-----------------------------------------------------------------------------
void GUIHEAP_GetHeapStats ( HeapStats_t *pxHeapStats )
{
  
  if( GUIHEAP_pblEnd == nullptr )
    {
    GUIHEAP_Init();
    }
  
  GUIBlockLink_t *pxBlock;
  /* portMAX_DELAY used as a portable way of getting the maximum value. */
  size_t xBlocks = 0, xMaxSize = 0, xMinSize = portMAX_DELAY;

  vTaskSuspendAll();
    {
    pxBlock = gStart.pxNextFreeBlock;

    /* pxBlock will be nullptr if the heap has not been initialised.  The heap
      is initialised automatically when the first allocation is made. */
    if( pxBlock != nullptr )
      {
      do
        {
        /* Increment the number of blocks and record the largest block seen so far. */
        xBlocks++;

        if( pxBlock->xBlockSize > xMaxSize )
          {
          xMaxSize = pxBlock->xBlockSize;
          }

        if( pxBlock->xBlockSize < xMinSize )
          {
          xMinSize = pxBlock->xBlockSize;
          }

        /* Move to the next block in the chain until the last block is
          reached. */
        pxBlock = pxBlock->pxNextFreeBlock;
        }
      while ( pxBlock != GUIHEAP_pblEnd );
      }
    }
  xTaskResumeAll();

  pxHeapStats->xSizeOfLargestFreeBlockInBytes  = xMaxSize;
  pxHeapStats->xSizeOfSmallestFreeBlockInBytes = xMinSize;
  pxHeapStats->xNumberOfFreeBlocks             = xBlocks;

  taskENTER_CRITICAL();
    {
    pxHeapStats->xAvailableHeapSpaceInBytes     = gGUIFreeBytesRemaining;
    pxHeapStats->xNumberOfSuccessfulAllocations = gGUINumberOfSuccessfulAllocations;
    pxHeapStats->xNumberOfSuccessfulFrees       = gGUINumberOfSuccessfulFrees;
    pxHeapStats->xMinimumEverFreeBytesRemaining = gGUIMinimumEverFreeBytesRemaining;
    }
  taskEXIT_CRITICAL();
}
//-----------------------------------------------------------------------------
