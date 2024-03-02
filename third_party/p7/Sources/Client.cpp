////////////////////////////////////////////////////////////////////////////////
//                                                                             /
// 2012-2020 (c) Baical                                                        /
//                                                                             /
// This library is free software; you can redistribute it and/or               /
// modify it under the terms of the GNU Lesser General Public                  /
// License as published by the Free Software Foundation; either                /
// version 3.0 of the License, or (at your option) any later version.          /
//                                                                             /
// This library is distributed in the hope that it will be useful,             /
// but WITHOUT ANY WARRANTY; without even the implied warranty of              /
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           /
// Lesser General Public License for more details.                             /
//                                                                             /
// You should have received a copy of the GNU Lesser General Public            /
// License along with this library.                                            /
//                                                                             /
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// This header file provide client functionality                               /
////////////////////////////////////////////////////////////////////////////////

#include "CommonClient.h"
#include "PSignal.h"
#include "Client.h"
#include "ClBaical.h"
#include "ClFile.h"
#include "ClText.h"
#include "ClNull.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                              Defines
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define CRASH_HANDLER_NAME                                           TM("Crash")
#define CRASH_LOCK_TIMEOUT                                           300000
#define CRASH_CLIENTS_COUNT                                          (8)
#define CLIENT_SHARED_PREFIX                                         TM("Cln_")

struct sCrash
{
    tINT32 volatile  lReference;
    CShared::hShared hShared;
    tUINT64          qwTimeStamp;
    CClient         *pClients[CRASH_CLIENTS_COUNT];
};


extern "C" 
{

////////////////////////////////////////////////////////////////////////////////
//P7_Create_Client
P7_EXPORT IP7_Client * __cdecl P7_Create_Client(const tXCHAR *i_pArgs)
{
    IP7_Client *l_pReturn   = NULL;
    int         l_iHC_ArgsC = 0;
    tXCHAR    **l_pHC_Args  = i_pArgs ? CProc::Get_ArgV(i_pArgs, &l_iHC_ArgsC) : NULL;
    int         l_iCn_ArgsC = 0;
    tXCHAR    **l_pCn_Args  = CProc::Get_ArgV(&l_iCn_ArgsC);
    int         l_iCount    = l_iCn_ArgsC + l_iHC_ArgsC;
    tXCHAR    **l_pArgs     = NULL;
    tXCHAR     *l_pSink     = NULL;
    tXCHAR     *l_pOn       = NULL;

    ////////////////////////////////////////////////////////////////////////////
    //Parse command line from console and from source code,
    //priority belongs to console
    if (l_iCount)
    {
        l_pArgs = (tXCHAR **)malloc(sizeof(tXCHAR *) * l_iCount);
        if (l_pArgs)
        {
            for (int l_iI = 0; l_iI < l_iCn_ArgsC; l_iI ++)
            {
                l_pArgs[l_iI] = l_pCn_Args[l_iI];
            }

            for (int l_iI = l_iCn_ArgsC; l_iI < l_iCount; l_iI ++)
            {
                l_pArgs[l_iI] = l_pHC_Args[l_iI - l_iCn_ArgsC];
            }
        }
        else
        {
            P7_Set_Last_Error(eP7_Error_MemoryAllocation);
        }
    }

    l_pSink = Get_Argument_Text_Value(l_pArgs, l_iCount, CLIENT_COMMAND_LINE_SINK);
    l_pOn   = Get_Argument_Text_Value(l_pArgs, l_iCount, CLIENT_COMMAND_LOG_ON);

    ////////////////////////////////////////////////////////////////////////////
    //select proper Sink
    if (    (NULL != l_pOn)
         && (TM('0') == l_pOn[0])
       )
    {
        //OFF
    }
    else if (    (NULL == l_pSink)
              || (0 == PStrICmp(l_pSink, CLIENT_SINK_BAICAL))
            )
    {
        l_pReturn = static_cast<IP7_Client *>(new CClBaical(l_pArgs, l_iCount));
    }
    else if (    (0 == PStrICmp(l_pSink, CLIENT_SINK_FILE_BIN))
              || (0 == PStrICmp(l_pSink, CLIENT_SINK_FILE_BIN_LEGACY))
            )
    {
        l_pReturn = static_cast<IP7_Client *>(new CClFile(l_pArgs, l_iCount));
    }
    else if (0 == PStrICmp(l_pSink, CLIENT_SINK_AUTO))
    {
        CClBaical *l_pBaical = new CClBaical(l_pArgs, l_iCount);
        if (    (l_pBaical)    
             && (    (ECLIENT_STATUS_OK != l_pBaical->Get_Status())
                  || (FALSE == l_pBaical->Connection_Wait(250))
                )
           )
        {
            delete l_pBaical;
            l_pBaical = NULL;
        }

        if (l_pBaical)
        {
            l_pReturn = static_cast<IP7_Client *>(l_pBaical);
        }
        else
        {
            l_pReturn = static_cast<IP7_Client *>(new CClFile(l_pArgs, l_iCount));
        }
    }
    else if (0 == PStrICmp(l_pSink, CLIENT_SINK_NULL))
    {
        l_pReturn = static_cast<IP7_Client *>(new CClNull(l_pArgs, l_iCount));
    }
    else //all other sink are text one
    //(0 == PStrICmp(l_pSink, CLIENT_SINK_FILE_TXT))
    //(0 == PStrICmp(l_pSink, CLIENT_SINK_CONSOLE))
    //(0 == PStrICmp(l_pSink, CLIENT_SINK_SYSLOG))
    {
        l_pReturn = static_cast<IP7_Client *>(new CClText(l_pArgs, l_iCount));
    }

    //if not initialized - remove
    if (    (l_pReturn)
         && (ECLIENT_STATUS_OK != l_pReturn->Get_Status())
       )
    {
        l_pReturn->Release();
        l_pReturn = NULL;
    }


    ////////////////////////////////////////////////////////////////////////////
    //do we need to print help
    if ( Get_Argument_Text_Value(l_pArgs, l_iCount,
                                 (const tXCHAR*)CLIENT_COMMAND_LOG_HELP
                                ) 
       )
    {
        PRINTF((tXCHAR*)CLIENT_HELP_STRING);
    }


    ////////////////////////////////////////////////////////////////////////////
    //cleanup
    if (l_pHC_Args)
    {
        CProc::Free_ArgV(l_pHC_Args);
        l_pHC_Args = NULL;
    }

    if (l_pCn_Args)
    {
        CProc::Free_ArgV(l_pCn_Args);
        l_pCn_Args = NULL;
    }

    if (l_pArgs)
    {
        free(l_pArgs);
        l_pArgs = NULL;
    }

    return l_pReturn;
}//P7_Create_Client


////////////////////////////////////////////////////////////////////////////////
//P7_Get_Shared
P7_EXPORT IP7_Client * __cdecl P7_Get_Shared(const tXCHAR *i_pName)
{
    IP7_Client   *l_pReturn  = NULL;
    tUINT32       l_dwLen1   = PStrLen(CLIENT_SHARED_PREFIX);
    tUINT32       l_dwLen2   = PStrLen(i_pName);
    tXCHAR       *l_pName    = (tXCHAR *)malloc(sizeof(tXCHAR) * (l_dwLen1 + l_dwLen2 + 16));
    tUINT32       l_uTimeHi  = 0;
    tUINT32       l_uTimeLo  = 0;
    sObjShared    l_stShared = {};
    CShared::hSem l_hSem     = SHARED_SEM_NULL;

    CProc::Get_Process_Time(&l_uTimeHi, &l_uTimeLo);

    if (l_pName)
    {
        PStrCpy(l_pName, l_dwLen1 + l_dwLen2 + 16, CLIENT_SHARED_PREFIX);
        PStrCpy(l_pName + l_dwLen1, l_dwLen2 + 16, i_pName);

        if (CShared::E_OK == CShared::Lock(l_pName, l_hSem, 250))
        {
            if (CShared::Read(l_pName, (tUINT8*)&l_stShared, sizeof(l_stShared)))
            {
                if (    (l_stShared.uProcTimeHi == l_uTimeHi)
                     && (l_stShared.uProcTimeLo == l_uTimeLo)
                   )
                {
                    l_pReturn = static_cast<IP7_Client *>(l_stShared.pPointer);
                    if (l_pReturn)
                    {
                        l_pReturn->Add_Ref();
                    }
                }
                else
                {
                    P7_Set_Last_Error(eP7_Error_SharedObjectInvalid);
                    CShared::UnLink(l_pName);
                }
            }

            CShared::UnLock(l_hSem);
        }

        free(l_pName);
        l_pName = NULL;
    }
    else
    {
        P7_Set_Last_Error(eP7_Error_MemoryAllocation);
    }

    return l_pReturn;
}//P7_Get_Shared


////////////////////////////////////////////////////////////////////////////////
//cbCrashHandler
void __cdecl cbCrashHandler(eCrashCode i_eCode, const void *i_pCrashContext, void *i_pUserContext)
{
    UNUSED_ARG(i_eCode);
    UNUSED_ARG(i_pCrashContext);
    UNUSED_ARG(i_pUserContext);
    P7_Exceptional_Flush();
}//cbCrashHandler


////////////////////////////////////////////////////////////////////////////////
//P7_Set_Crash_Handler
P7_EXPORT void __cdecl P7_Set_Crash_Handler()
{
    ChInstall();
    ChSetHandler(&cbCrashHandler);
}//P7_Set_Crash_Handler


////////////////////////////////////////////////////////////////////////////////
//P7_Clr_Crash_Handler
P7_EXPORT void __cdecl P7_Clr_Crash_Handler()
{
    ChUnInstall();
}//P7_Clr_Crash_Handler


////////////////////////////////////////////////////////////////////////////////
//P7_Exceptional_Flush
//0) Lock crash memory
//1) Read crash memory
//2) Call every client crash functions
//3)  - every client set "Error" flag to prevent ext. access (add data, add/del channel)
//4)  - every client call crash function on every channel
//5)    - some channels will dump stack back trace & exception info
//6)    - every channel close connection & set "Error" flag
//7)  - every client write/deliver all buffers
//8)  - every client release shared memory
//9) unlock crash memory
//10) release crash memory
//11) leave function to execute def. handler
P7_EXPORT void __cdecl P7_Exceptional_Flush()
{
    tBOOL         l_bLock      = FALSE;
    sCrash        l_sCrash     = {}; 
    tUINT32       l_uTimeHi    = 0;
    tUINT32       l_uTimeLo    = 0;
    tUINT64       l_qwProcTime = 0;
    tBOOL         l_bTimeError = FALSE;
    CShared::hSem l_hSem       = SHARED_SEM_NULL;


    if (CProc::Get_Process_Time(&l_uTimeHi, &l_uTimeLo))
    {
        l_qwProcTime = ((tUINT64)l_uTimeHi << 32) + (tUINT64)l_uTimeLo;
    }

    ////////////////////////////////////////////////////////////////////////////
    //trying to block shared memory
    if (CShared::E_OK != CShared::Lock(CRASH_HANDLER_NAME, l_hSem, CRASH_LOCK_TIMEOUT))
    {
        goto l_lblExit;
    }

    l_bLock = TRUE;

    if (FALSE == CShared::Read(CRASH_HANDLER_NAME, 
                               (tUINT8*)&l_sCrash, 
                               (tUINT16)sizeof(l_sCrash)
                              )
       )
    {
        goto l_lblExit;
    }

    if (l_sCrash.qwTimeStamp == l_qwProcTime)
    {
        for (tUINT32 l_dwI = 0; l_dwI < CRASH_CLIENTS_COUNT; l_dwI++)
        {
            if (l_sCrash.pClients[l_dwI])
            {
                l_sCrash.pClients[l_dwI]->Unshare();
                l_sCrash.pClients[l_dwI]->Close();
            }
        }
    }
    else
    {
        l_bTimeError = TRUE;
    }

l_lblExit:
    if (l_bTimeError)
    {
        CShared::UnLink(CRASH_HANDLER_NAME);
    }
    else
    {
        CShared::Close(l_sCrash.hShared);
    }

    if (l_bLock)
    {
        CShared::UnLock(l_hSem);
    }
}//P7_Exceptional_Flush


////////////////////////////////////////////////////////////////////////////////
//P7_Flush
//0) Lock crash memory
//1) Read crash memory
//2) Call every client crash functions
//3)  - every client set "Error" flag to prevent ext. access (add data, add/del channel)
//4)  - every client call crash function on every channel
//5)    - some channels will dump stack back trace & exception info
//6)    - every channel close connection & set "Error" flag
//7)  - every client write/deliver all buffers
//8)  - every client release shared memory
//9) unlock crash memory
//10) release crash memory
//11) leave function to execute def. handler
P7_EXPORT void __cdecl P7_Flush()
{
    tBOOL         l_bLock      = FALSE;
    sCrash        l_sCrash     = {};
    tUINT32       l_uTimeHi    = 0;
    tUINT32       l_uTimeLo    = 0;
    tUINT64       l_qwProcTime = 0;
    CShared::hSem l_hSem       = SHARED_SEM_NULL;


    if (CProc::Get_Process_Time(&l_uTimeHi, &l_uTimeLo))
    {
        l_qwProcTime = ((tUINT64)l_uTimeHi << 32) + (tUINT64)l_uTimeLo;
    }

    ////////////////////////////////////////////////////////////////////////////
    //trying to block shared memory
    if (CShared::E_OK != CShared::Lock(CRASH_HANDLER_NAME, l_hSem, CRASH_LOCK_TIMEOUT))
    {
        goto l_lblExit;
    }

    l_bLock = TRUE;

    if (FALSE == CShared::Read(CRASH_HANDLER_NAME, 
                               (tUINT8*)&l_sCrash, 
                               (tUINT16)sizeof(l_sCrash)
                              )
       )
    {
        goto l_lblExit;
    }

    if (l_sCrash.qwTimeStamp == l_qwProcTime)
    {
        for (tUINT32 l_dwI = 0; l_dwI < CRASH_CLIENTS_COUNT; l_dwI++)
        {
            if (l_sCrash.pClients[l_dwI])
            {
                l_sCrash.pClients[l_dwI]->Flush();
            }
        }
    }

l_lblExit:
    if (l_bLock)
    {
        CShared::UnLock(l_hSem);
    }
}//P7_Exceptional_Flush


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Errors processing routines                          //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct stP7_Error_Thread
{
    tUINT32  uThreadId;
    eP7_Error eError;
};

class CTreeThreadErrors: public CRBTree<stP7_Error_Thread*, tUINT32>
{
public:
    virtual ~CTreeThreadErrors() { Clear(); }

protected:
    virtual tBOOL Data_Release(stP7_Error_Thread *i_pData) { free(i_pData); return TRUE; }
    //Return TRUE  - if (i_pKey < i_pData::key), FALSE - otherwise
    tBOOL Is_Key_Less(tUINT32 i_pKey, stP7_Error_Thread *i_pData) { return (i_pKey < i_pData->uThreadId); }
    //Return TRUE  - if (i_pKey == i_pData::key), FALSE - otherwise
    tBOOL Is_Qual(tUINT32 i_pKey, stP7_Error_Thread *i_pData) { return (i_pKey == i_pData->uThreadId); }
};


static CTreeThreadErrors g_cErrorsTree;
static CLock             g_cErrorLock; 

////////////////////////////////////////////////////////////////////////////////
P7_EXPORT eP7_Error __cdecl P7_Last_Error()
{
    CLock l_cLock(&g_cErrorLock);

    eP7_Error l_eReturn = eP7_Error_None;

    stP7_Error_Thread *l_pError = g_cErrorsTree.Find(CProc::Get_Thread_Id());
    if (l_pError)
    {
        l_eReturn = l_pError->eError;
        l_pError->eError = eP7_Error_None;
    }

    return l_eReturn;
}

////////////////////////////////////////////////////////////////////////////////
void __cdecl P7_Set_Last_Error(eP7_Error i_eError)
{
    CLock l_cLock(&g_cErrorLock);

    tUINT32   l_uThreadId = CProc::Get_Thread_Id();

    stP7_Error_Thread *l_pError = g_cErrorsTree.Find(l_uThreadId);
    if (l_pError)
    {
        l_pError->eError = i_eError;
    }
    else
    {
        l_pError = (stP7_Error_Thread *)malloc(sizeof(stP7_Error_Thread));
        l_pError->eError = i_eError;
        l_pError->uThreadId = l_uThreadId;
        g_cErrorsTree.Push(l_pError, l_uThreadId);
    }
}
}//extern "C"


////////////////////////////////////////////////////////////////////////////////
//Get_Argument_Text_Value
tXCHAR *Get_Argument_Text_Value(tXCHAR       **i_pArgs,
                                tINT32         i_iCount,
                                const tXCHAR  *i_pName
                               )
{
    tXCHAR *l_pReturn = NULL;

    if (    (NULL == i_pName)
         || (NULL == i_pArgs) 
         || (0 >= i_iCount) 
       )
    {
        return l_pReturn;
    }

    tUINT32 l_dwName_Lenght = PStrLen(i_pName);
    tUINT32 i_dwArg_Lenght  = 0; 

    for(tINT32 l_iIDX = 0; l_iIDX < i_iCount; l_iIDX++)
    {
        i_dwArg_Lenght = PStrLen(i_pArgs[l_iIDX]); 

        if (    (l_dwName_Lenght <= i_dwArg_Lenght)
                && (0 == PStrNCmp(i_pArgs[l_iIDX], i_pName, l_dwName_Lenght))
           )
        {
            l_pReturn = i_pArgs[l_iIDX] + l_dwName_Lenght;
            break;
        }
    }

    return l_pReturn;
}//Get_Argument_Text_Value



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                              CClient
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
CClient::CClient(IP7_Client::eType i_eType,
                 tXCHAR          **i_pArgs,
                 tINT32            i_iCount
                )
    : m_lReference(1)
    , m_eStatus(ECLIENT_STATUS_OK)
    , m_hShared(NULL)
    , m_bCrashMem(FALSE)
    , m_pLog(NULL)
    , m_bConnected(TRUE)
    , m_dwConnection_Resets(0)
    , m_eType(i_eType)
    , m_pArgs(NULL)
    , m_iArgsCnt(0)
    , m_bFlushChannels(TRUE)

{
    memset(m_pChannels, 0, sizeof(IP7C_Channel*)*USER_PACKET_CHANNEL_ID_MAX_SIZE);
    memset(&m_hCS,      0, sizeof(m_hCS));
    memset(&m_hCS_Reg,  0, sizeof(m_hCS_Reg));

    if (    (i_pArgs)
         && (i_iCount)
       )
    {
        m_iArgsCnt = i_iCount;
        m_pArgs    = (tXCHAR **)malloc(sizeof(tXCHAR *) * m_iArgsCnt);

        if (m_pArgs)
        {
            for (int l_iI = 0; l_iI < m_iArgsCnt; l_iI ++)
            {
                m_pArgs[l_iI] = PStrDub(i_pArgs[l_iI]);
            }
        }
    }


    tXCHAR *l_pFlush = Get_Argument_Text_Value(m_pArgs, m_iArgsCnt, CLIENT_COMMAND_FLUSH_CHANNELS);

    if (    (NULL != l_pFlush)
         && (TM('0') == l_pFlush[0])
       )
    {
        m_bFlushChannels = FALSE;
    }

    LOCK_CREATE(m_hCS_Reg);
    LOCK_CREATE(m_hCS);

    Init_Log(i_pArgs, i_iCount);
}//CClient


////////////////////////////////////////////////////////////////////////////////
//~CClient
CClient::~CClient()
{
    //remove log
    if (m_pLog)
    {
        m_pLog->Release();
        m_pLog = NULL;
    }

    if (m_pArgs)
    {
        for (int l_iI = 0; l_iI < m_iArgsCnt; l_iI ++)
        {
            PStrFreeDub(m_pArgs[l_iI]);
            m_pArgs[l_iI] = NULL;
        }

        free(m_pArgs);
        m_pArgs    = NULL;
        m_iArgsCnt = 0;
    }

    LOCK_DESTROY(m_hCS_Reg);
    LOCK_DESTROY(m_hCS);
}//~CClient


////////////////////////////////////////////////////////////////////////////////
//Get_Status()
eClient_Status CClient::Get_Status()
{
    return m_eStatus;
}//Get_Status()


////////////////////////////////////////////////////////////////////////////////
//Get_Status
tBOOL CClient::Get_Status(sP7C_Status *o_pStatus)
{
    if (NULL == o_pStatus)
    {
        return FALSE;
    }

    LOCK_ENTER(m_hCS);
    o_pStatus->bConnected = m_bConnected;
    o_pStatus->dwResets   = m_dwConnection_Resets;
    LOCK_EXIT(m_hCS);

    return TRUE;
}//Get_Status


////////////////////////////////////////////////////////////////////////////////
//Get_Type()
IP7_Client::eType CClient::Get_Type()
{
    return m_eType;
}//Get_Type()


////////////////////////////////////////////////////////////////////////////////
//Add_Ref()
tINT32 CClient::Add_Ref()
{
    return ATOMIC_INC(&m_lReference);
}//Add_Ref()


////////////////////////////////////////////////////////////////////////////////
//Release()
tINT32 CClient::Release()
{                       
    tINT32 l_lResult = ATOMIC_DEC(&m_lReference);
    if ( 0 >= l_lResult )
    {
        delete this;
    }

    return l_lResult;
}//Release()


////////////////////////////////////////////////////////////////////////////////
//Register_Channel()
eClient_Status CClient::Register_Channel(IP7C_Channel *i_pChannel)
{
    eClient_Status  l_eReturn = ECLIENT_STATUS_INTERNAL_ERROR;

    if (NULL == i_pChannel)
    {
        return ECLIENT_STATUS_WRONG_PARAMETERS;
    }

    sP7C_Status l_sStatus = {FALSE, 0};

    this->Get_Status(&l_sStatus);

    LOCK_ENTER(m_hCS_Reg);

    for (tUINT32 l_dwI = 0; l_dwI < USER_PACKET_CHANNEL_ID_MAX_SIZE; l_dwI++)
    {
        if (NULL == m_pChannels[l_dwI])
        {
            sP7C_Channel_Info l_sInfo = {l_dwI};

            m_pChannels[l_dwI] = i_pChannel;
            
            i_pChannel->On_Init(&l_sInfo);
            i_pChannel->On_Status(l_dwI, &l_sStatus);

            l_eReturn = ECLIENT_STATUS_OK;

            break;
        }
    }

    LOCK_EXIT(m_hCS_Reg);

    return l_eReturn;
}//Register_Channel()


////////////////////////////////////////////////////////////////////////////////
//Unregister_Channel()
eClient_Status CClient::Unregister_Channel(tUINT32 i_dwID)
{
    eClient_Status  l_eReturn = ECLIENT_STATUS_OK;

    LOCK_ENTER(m_hCS_Reg);

    if (    (i_dwID >= USER_PACKET_CHANNEL_ID_MAX_SIZE)
         || (NULL   == m_pChannels[i_dwID])
       )
    {
        l_eReturn = ECLIENT_STATUS_WRONG_PARAMETERS;
    }
    else
    {
        m_pChannels[i_dwID] = NULL;
    }

    LOCK_EXIT(m_hCS_Reg);

    return l_eReturn;
}//Unregister_Channel()


////////////////////////////////////////////////////////////////////////////////
//Share()
tBOOL CClient::Share(const tXCHAR *i_pName)
{
    tBOOL l_bReturn = FALSE;

    LOCK_ENTER(m_hCS_Reg);

    if (NULL == m_hShared)
    {
        tUINT32 l_dwLen1 = PStrLen(CLIENT_SHARED_PREFIX);
        tUINT32 l_dwLen2 = PStrLen(i_pName);
        tXCHAR *l_pName = (tXCHAR *)malloc(sizeof(tXCHAR) * (l_dwLen1 + l_dwLen2 + 16));

        if (l_pName)
        {
            sObjShared    l_stShared = {};
            tUINT32       l_uTimeHi  = 0;
            tUINT32       l_uTimeLo  = 0;
            tBOOL         l_bCreate  = TRUE;
            CShared::hSem l_hSem     = SHARED_SEM_NULL;

            CProc::Get_Process_Time(&l_uTimeHi, &l_uTimeLo);

            PStrCpy(l_pName, l_dwLen1 + l_dwLen2 + 16, CLIENT_SHARED_PREFIX);
            PStrCpy(l_pName + l_dwLen1, l_dwLen2 + 16, i_pName);

            JOURNAL_WARNING(m_pLog, TM("Shared memory {%s} registration error"), l_pName);
            if (CShared::E_OK == CShared::Lock(l_pName, l_hSem, 250))
            {
                l_bCreate = FALSE; //it is already existing

                if (CShared::Read(l_pName, (tUINT8*)&l_stShared, sizeof(l_stShared)))
                {
                    if (    (l_stShared.uProcTimeHi != l_uTimeHi)
                         || (l_stShared.uProcTimeLo != l_uTimeLo)
                       )
                    {
                        JOURNAL_ERROR(m_pLog, TM("Shared memory timestamp error, prev. session crashed or forget to release P7 objects?"));
                        CShared::UnLink(l_pName);
                        l_bCreate = TRUE;
                    }
                }
                CShared::UnLock(l_hSem);
            }

            if (l_bCreate)
            {
                CProc::Get_Process_Time(&l_uTimeHi, &l_uTimeLo);
                l_stShared.pPointer    = static_cast<IP7_Client*>(this);
                l_stShared.uProcTimeHi = l_uTimeHi;
                l_stShared.uProcTimeLo = l_uTimeLo;
                l_bReturn = CShared::Create(&m_hShared, l_pName, (tUINT8*)&l_stShared, sizeof(l_stShared));
            }

            free(l_pName);
            l_pName = NULL;
        }
    }

    LOCK_EXIT(m_hCS_Reg); 

    return l_bReturn;
}//Share()


////////////////////////////////////////////////////////////////////////////////
//Get_Argument()
const tXCHAR *CClient::Get_Argument(const tXCHAR  *i_pName)
{
    return Get_Argument_Text_Value(m_pArgs, m_iArgsCnt, i_pName);
}


////////////////////////////////////////////////////////////////////////////////
//Get_Channels_Count()
size_t CClient::Get_Channels_Count()
{
    size_t l_szReturn = 0;

    LOCK_ENTER(m_hCS_Reg);

    for (tUINT32 l_dwI = 0; l_dwI < USER_PACKET_CHANNEL_ID_MAX_SIZE; l_dwI++)
    {
        if (m_pChannels[l_dwI])
        {
            l_szReturn ++;
        }
    }
    LOCK_EXIT(m_hCS_Reg);

    return l_szReturn;
}//Get_Channels_Count()


////////////////////////////////////////////////////////////////////////////////
//Get_Channel()
IP7C_Channel *CClient::Get_Channel(size_t i_szIndex)
{
    IP7C_Channel *l_pReturn = NULL;
    size_t        l_szCount = 0;

    LOCK_ENTER(m_hCS_Reg);

    for (tUINT32 l_dwI = 0; l_dwI < USER_PACKET_CHANNEL_ID_MAX_SIZE; l_dwI++)
    {
        if (m_pChannels[l_dwI])
        {
            if (l_szCount == i_szIndex)
            {
                l_pReturn = m_pChannels[l_dwI];
                l_pReturn->Add_Ref();
                break;
            }
            else
            {
                l_szCount ++;
            }
        }
    }
    LOCK_EXIT(m_hCS_Reg);

    return l_pReturn;
}//Get_Channel()


////////////////////////////////////////////////////////////////////////////////
//Unshare()
tBOOL CClient::Unshare()
{
    LOCK_ENTER(m_hCS_Reg);
    if (m_hShared)
    {
        const tXCHAR *l_pName = CShared::GetName(m_hShared);
        CShared::hSem l_hSem  = SHARED_SEM_NULL;

        if (    (l_pName)
             && (CShared::E_OK == CShared::Lock(l_pName, l_hSem, CRASH_LOCK_TIMEOUT))
           )
        {
            CShared::Close(m_hShared);
            m_hShared = NULL;
            CShared::UnLock(l_hSem);
        }
        else
        {
            JOURNAL_ERROR(m_pLog, TM("Can't destroy shared memory"));
        }
    }
    LOCK_EXIT(m_hCS_Reg); 

    return TRUE;
}//Unshare()


////////////////////////////////////////////////////////////////////////////////
//Init_Log
eClient_Status CClient::Init_Log(tXCHAR **i_pArgs, tINT32 i_iCount)
{
    tXCHAR          *l_pArg_Value   = NULL;
    IJournal::eLevel l_eVerbosity   = IJournal::eLEVEL_CRITICAL;

    l_pArg_Value = Get_Argument_Text_Value(i_pArgs, i_iCount,
                                           (const tXCHAR*)CLIENT_COMMAND_LOG_VERBOSITY
                                          );
    if (NULL == l_pArg_Value)
    {
        goto l_lClean_Up;
    }

    l_eVerbosity = (IJournal::eLevel)PStrToInt(l_pArg_Value);

    if (IJournal::eLEVEL_COUNT <= l_eVerbosity)
    {
        goto l_lClean_Up;
    }


    m_pLog = new CJournal();

    if (m_pLog)
    {
        if (FALSE == m_pLog->Initialize(TM("P7.Logs\\")))
        {
            m_pLog->Release();
            m_pLog = NULL;
        }
        else
        {
            m_pLog->Set_Verbosity(l_eVerbosity);
        }
    }

l_lClean_Up:

    return ECLIENT_STATUS_OK;
}//Init_Log



////////////////////////////////////////////////////////////////////////////////
//Init_Crash_Handler()
//1) lock shared memory (infinite)
//   - success - goto 5
//   - failed  - goto 2
//------------------------------------
//2) Fill sCrash structure
//3) Try to create&write shared memory
//   - success - register Signal & exit
//   - failed  - goto 4
//------------------------------------
//4) lock shared memory (infinite)
//   - success - goto 5
//   - failed  - exit, it is error
//------------------------------------
//5) Read shared memory (without lock)
//6) Update shared memory by new client
//7) increment reference counter
//8) write back shared memory
//9) Unlock shared memory

eClient_Status CClient::Init_Crash_Handler(tXCHAR **i_pArgs, tINT32 i_iCount)
{
    CShared::eLock l_eLock      = CShared::E_OK;
    tBOOL          l_bResult    = FALSE;
    sCrash         l_sCrash; //not initialized here
    tUINT32        l_uTimeHi    = 0;
    tUINT32        l_uTimeLo    = 0;
    tUINT64        l_qwProcTime = 0;
    CShared::hSem  l_hSem       = SHARED_SEM_NULL;

    UNUSED_ARG(i_pArgs);
    UNUSED_ARG(i_iCount);

    if (CProc::Get_Process_Time(&l_uTimeHi, &l_uTimeLo))
    {
        l_qwProcTime = ((tUINT64)l_uTimeHi << 32) + (tUINT64)l_uTimeLo;
    }

    ////////////////////////////////////////////////////////////////////////////
    //trying to block shared memory
    l_eLock = CShared::Lock(CRASH_HANDLER_NAME, l_hSem, CRASH_LOCK_TIMEOUT); //5 minutes

    if (CShared::E_OK == l_eLock)
    {
        //if we are here it mean we are successfully get lock and now we need to read
        //memory, update it and release lock
        if (CShared::Read(CRASH_HANDLER_NAME, (tUINT8*)&l_sCrash, (tUINT16)sizeof(l_sCrash)))
        {
            if (l_sCrash.qwTimeStamp == l_qwProcTime)
            {
                for (tUINT32 l_dwI = 0; l_dwI < CRASH_CLIENTS_COUNT; l_dwI++)
                {
                    if (NULL == l_sCrash.pClients[l_dwI])
                    {
                        l_sCrash.pClients[l_dwI] = this;
                        l_sCrash.lReference ++;
                        l_bResult = TRUE;
                        break;
                    }
                }

                //no free room in shared memory, exit !
                if (l_bResult)
                {
                    if (FALSE == CShared::Write(CRASH_HANDLER_NAME,
                                                (tUINT8*)&l_sCrash,
                                                (tUINT16)sizeof(l_sCrash)
                                               )
                       )
                    {
                        JOURNAL_ERROR(m_pLog, TM("Can't write to crash shared memory"));
                        l_bResult = FALSE;
                    }
                }
                else
                {
                    JOURNAL_ERROR(m_pLog, TM("There is no free space for client"));
                }
            }
            else
            {
                JOURNAL_ERROR(m_pLog, TM("Shared memory timestamp missmatch, it wasn't closed properly last time?"));
                CShared::UnLink(CRASH_HANDLER_NAME);
                l_eLock = CShared::E_NOT_EXISTS; //going to create new shared memory space
            }
        }
        else
        {
            JOURNAL_ERROR(m_pLog, TM("Can't read crash shared memory"));
        }

        CShared::UnLock(l_hSem);
    } //if (CShared::E_OK == l_eLock)

    if (CShared::E_NOT_EXISTS == l_eLock)
    {
        //it is not exists - create new one/////////////////////////////////////
        memset(&l_sCrash, 0, sizeof(l_sCrash));
        l_sCrash.lReference  = 1;
        l_sCrash.qwTimeStamp = l_qwProcTime;
        l_sCrash.pClients[0] = this;
        
        JOURNAL_INFO(m_pLog, TM("Try to create shared lock"));

        if (CShared::Create(&l_sCrash.hShared,
                            CRASH_HANDLER_NAME,
                            (tUINT8*)&l_sCrash,
                            (tUINT16)sizeof(l_sCrash)
                           )
           )
        {
            //successful creation of shared memory/////////////////////////////
            JOURNAL_INFO(m_pLog, TM("Register new crash handler"));
            l_bResult = TRUE;
        }
        else //creation of shared memory failed 
        {
            JOURNAL_WARNING(m_pLog, TM("CShared::Create failed, try to lock"));
            //it wasn't possible to create shared memory, possibly it was created
            //by somebody else, trying to get lock
            l_eLock = CShared::Lock(CRASH_HANDLER_NAME, l_hSem, CRASH_LOCK_TIMEOUT); //5 minutes
            if (CShared::E_OK == l_eLock)
            {
                //if we are here it mean we are successfully get lock and now we need to read
                //memory, update it and release lock
                if (CShared::Read(CRASH_HANDLER_NAME, (tUINT8*)&l_sCrash, (tUINT16)sizeof(l_sCrash)))
                {
                    if (l_sCrash.qwTimeStamp == l_qwProcTime)
                    {
                        for (tUINT32 l_dwI = 0; l_dwI < CRASH_CLIENTS_COUNT; l_dwI++)
                        {
                            if (NULL == l_sCrash.pClients[l_dwI])
                            {
                                l_sCrash.pClients[l_dwI] = this;
                                l_sCrash.lReference ++;
                                l_bResult = TRUE;
                                break;
                            }
                        }

                        //no free room in shared memory, exit !
                        if (l_bResult)
                        {
                            if (FALSE == CShared::Write(CRASH_HANDLER_NAME,
                                                        (tUINT8*)&l_sCrash,
                                                        (tUINT16)sizeof(l_sCrash)
                                                       )
                               )
                            {
                                JOURNAL_ERROR(m_pLog, TM("Can't write to crash shared memory"));
                                l_bResult = FALSE;
                            }
                        }
                    }
                    else //if (l_sCrash.qwTimeStamp != l_qwProcTime)
                    {
                        JOURNAL_ERROR(m_pLog, TM("Shared memory timestamp missmatch, it wasn't closed properly last time?"));
                        CShared::UnLink(CRASH_HANDLER_NAME);
                        l_eLock = CShared::E_NOT_EXISTS; //going to create new shared memory space
                    }
                }
                else
                {
                    JOURNAL_ERROR(m_pLog, TM("Can't read crash shared memory"));
                    l_bResult = FALSE;
                }

                CShared::UnLock(l_hSem);
            }
            else
            {
                JOURNAL_ERROR(m_pLog, TM("Can't get lock for crash handler"));
                l_bResult = FALSE;
            }
        }
    } //if (CShared::E_NOT_EXISTS == l_eLock) 
    else  if (CShared::E_OK != l_eLock)
    {
        //if it is internal errors//////////////////////////////////////////////
        JOURNAL_ERROR(m_pLog, TM("Can't get lock for crash handler"));
        l_bResult = FALSE;
    }

    if (l_bResult)
    {
        LOCK_ENTER(m_hCS_Reg);
        m_bCrashMem = TRUE;
        LOCK_EXIT(m_hCS_Reg);
    }

    return ECLIENT_STATUS_OK;
}//Init_Crash_Handler

    
////////////////////////////////////////////////////////////////////////////////
//Del_Crash_Handler()
//1) lock shared memory (infinite)
//   - success - goto 2
//   - failed  - exit
//2) Read shared memory
//3) find client and clear pointer
//4) decrease reference counter
//5) if reference counter = 0 delete shared memory & unregister signal
//6) unlock shared memory (if memory was deleted - it will just return error)
eClient_Status CClient::Uninit_Crash_Handler()
{
    tBOOL         l_bReturn   = FALSE;
    tBOOL         l_bLock     = FALSE;
    tBOOL         l_bCrashMem = FALSE;
    sCrash        l_sCrash; //not initialized here
    CShared::hSem l_hSem      = SHARED_SEM_NULL;

    LOCK_ENTER(m_hCS_Reg);
    l_bCrashMem = m_bCrashMem;
    LOCK_EXIT(m_hCS_Reg);

    if (FALSE == l_bCrashMem)
    {
        l_bReturn = FALSE;
        goto l_lblExit;
    }

    ////////////////////////////////////////////////////////////////////////////
    //trying to block shared memory
    if (CShared::E_OK != CShared::Lock(CRASH_HANDLER_NAME, l_hSem, CRASH_LOCK_TIMEOUT))
    {
        JOURNAL_ERROR(m_pLog, TM("Can't get lock for crash handler"));
        l_bReturn = FALSE;
        goto l_lblExit;
    }

    l_bLock = TRUE;

    if (FALSE == CShared::Read(CRASH_HANDLER_NAME, 
                               (tUINT8*)&l_sCrash, 
                               (tUINT16)sizeof(l_sCrash)
                              )
       )
    {
        JOURNAL_ERROR(m_pLog, TM("Can't read from crash shared memory"));
        l_bReturn = FALSE; 
        goto l_lblExit;
    }

    for (tUINT32 l_dwI = 0; l_dwI < CRASH_CLIENTS_COUNT; l_dwI++)
    {
        if (this == l_sCrash.pClients[l_dwI])
        {
            l_sCrash.pClients[l_dwI] = NULL;
            l_bReturn                = TRUE;
            l_sCrash.lReference --;
            break;
        }
    }

    //client is not found
    if (FALSE == l_bReturn)
    {
        goto l_lblExit;
    }

    if (l_sCrash.lReference)
    {
        if (FALSE == CShared::Write(CRASH_HANDLER_NAME, 
                                    (tUINT8*)&l_sCrash, 
                                    (tUINT16)sizeof(l_sCrash)
                                   )
           )
        {
            JOURNAL_ERROR(m_pLog, TM("Can't write to crash shared memory"));
            l_bReturn = FALSE; 
            goto l_lblExit;
        }
    }
    else
    {
        CShared::Close(l_sCrash.hShared);
    }

l_lblExit:

    if (l_bLock)
    {
        CShared::UnLock(l_hSem);
    }

    return ECLIENT_STATUS_OK;
}//Del_Crash_Handler()


