/*-------------------------------------------------------------------
-------------------------------------------------------------------*/
#ifndef TOOL_TYPE_H
#define TOOL_TYPE_H
//-------------------------------------------------------------------
#define TOOL_VAK_8              0x00
#define TOOL_VAK_8U             0x01
#define TOOL_AKC                0x02
#define TOOL_VAK_8_M_09         0x03
#define TOOL_AKC_SD             0x04
#define TOOL_AKC_MF             0x05
#define TOOL_AKC_MF_SD          0x06
#define TOOL_VAK_8_SL           0x07
#define TOOL_SAS_BP_SL          0x08
#define TOOL_VAK32              0x09
#define TOOL_AKC8S_FAST_SD  	0x0A
#define TOOL_AKC4S_FAST_SD  	0x0B
#define TOOL_VAK32_SHDSL        0x0C
#define TOOL_AKP_52             0x0D
#define TOOL_AKP_76			0x0E
#define TOOL_UNKNOW             0x0FF
//-------------------------------------------------------------------
enum TTOOL_TYPE {   VAK_8           = TOOL_VAK_8,
                    VAK_8U          = TOOL_VAK_8U,
                    AKC             = TOOL_AKC,
                    VAK_8_M_09      = TOOL_VAK_8_M_09,
                    AKC_SD          = TOOL_AKC_SD,
                    AKC_MF          = TOOL_AKC_MF,
                    AKC_MF_SD       = TOOL_AKC_MF_SD,
                    VAK_8_SL        = TOOL_VAK_8_SL,
                    SAS_BP_SL       = TOOL_SAS_BP_SL,
                    VAK32           = TOOL_VAK32,
                    AKC8S_FAST_SD   = TOOL_AKC8S_FAST_SD,
                    AKC4S_FAST_SD   = TOOL_AKC4S_FAST_SD,
                    VAK32_SHDSL     = TOOL_VAK32_SHDSL,
                    AKP_52          = TOOL_AKP_52,
                    AKP_76          = TOOL_AKP_76,
                    UNKNOWN};
//-------------------------------------------------------------------
#endif //TOOL_TYPE_H
//-------------------------------------------------------------------
