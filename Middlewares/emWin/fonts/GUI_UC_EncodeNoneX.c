//-----------------------------------------------------------------------------
/*
 File        : GUI_UC_EncodeNoneX.c
 Version     : V1.10
 By          : 银网科技

 For         : emWin
 Description :替换emWIN中GUI_UC_EncodeNone单元中的功能
        
 Date       : 2023.12.05
*/
//-----------------------------------------------------------------------------
#include "GUI_Type.h"

//-----------------------------------------------------------------------------
/*
 *       Static code
 *
*/ 
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
 *       _GetCharCode
 *
 * Purpose:
 *   Return the UNICODE character code of the current character.
*/ 
//-----------------------------------------------------------------------------
static U16 _GetCharCode(const char GUI_UNI_PTR * s) 
{

  U16 uwRes;
  
  if( !s )
    uwRes = 0x20;
  else if( ((U8*)s)[0] < 0xA0 )
    uwRes = s[0];
  else
    uwRes = (((U16)((U8*)s)[0]) << 8 ) + ((U8*)s)[1];
 
  return uwRes;
}

 /*********************************************************************
 *
 *       _GetCharSize
 *
 * Purpose:
 *   Return the number of bytes of the current character.
 */
 static int _GetCharSize(const char GUI_UNI_PTR * s) 
 {
   
   if( ((U8*)s)[0] > 0xA0 ) 
     return 2; 
     
   return 1;
 }

 /*********************************************************************
 *
 *       _CalcSizeOfChar
 *
 * Purpose:
 *   Return the number of bytes needed for the given character.
 */
 static int _CalcSizeOfChar(U16 uwChar) 
 {

   if( uwChar > 0xA0A0)                
     return 2;
   return 1;
 }

 /*********************************************************************
 *
 *       _Encode
 *
 * Purpose:
 *   Encode character into 1/2/3 bytes.
 */
 static int _Encode(char *s, U16 Char) 
 {

   if(Char > 0xA0A0)
     {
     s[0] = Char >> 8;
     s[1] = Char;
     return 2;
     }
     
   *s = (U8)(Char);
   return 1;
 }

 /*********************************************************************
 *
 *       Static data
 *
 **********************************************************************
 */
 /*********************************************************************
 *
 *       _API_Table
 */
 const GUI_UC_ENC_APILIST GUI_UC_None = {
   _GetCharCode,     /*  return character code as U16 */
   _GetCharSize,     /*  return size of character: 1 */
   _CalcSizeOfChar,  /*  return size of character: 1 */
   _Encode           /*  Encode character */
 };
 /*************************** End of file ****************************/
