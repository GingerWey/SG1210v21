//-----------------------------------------------------------------------------
/*
 File        : GDialogRes.h
 Version     : V2.00
 By          : Wey@SilverGrid

 Description : Dialog resource configurations for GConfigForm
               All dialog resources exported as extern declarations
               Per SG1210v25对话框.md specification

 Date        : 2026.07.21 (V2.00 - refactor to extern declarations, add GListbox)
               2026.07.21 (V1.00 - initial implementation with getter functions)
*/
//-----------------------------------------------------------------------------
#ifndef GDIALOGRES_H
#define GDIALOGRES_H

#include "GDialog.h"
#include "GListbox.h"
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//=============================================================================
// Dialog Resource Declarations
//=============================================================================

// GNumRegDialog configuration (§5)
extern const GDialog::GDialogConfig g_numRegDialogConfig;

// GLoginDialog configuration (§6)
extern const GDialog::GDialogConfig g_loginDialogConfig;

// GIPAddressDialog configuration (§7)
extern const GDialog::GDialogConfig g_ipDialogConfig;

// GDatetimeDialog configuration (§8)
extern const GDialog::GDialogConfig g_datetimeDialogConfig;

// GListbox style (§9)
extern const GListbox::GStyle g_listboxStyle;

//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------
#endif // GDIALOGRES_H
