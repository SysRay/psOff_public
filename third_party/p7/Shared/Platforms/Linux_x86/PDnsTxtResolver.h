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
#include <netinet/in.h>
//#include <arpa/nameser.h>
#include <resolv.h>
#include <netdb.h>

#include "GTypes.h"
#include "AList.h"

class CDnsTxt: public CAList<tACHAR*>
{
protected:
    virtual tBOOL Data_Release(tACHAR *i_pStr)
    {
        if (NULL == i_pStr)
        {
            return FALSE;
        }

        free(i_pStr);
        return TRUE;
    }//CListBase::Data_Release
};


tBOOL static inline GetDnsTxtRecords(const tACHAR *i_pDomain, CDnsTxt &io_rList)
{
    const size_t l_szTxtBuf = 2048;
    u_char       l_pTxtBuf[l_szTxtBuf];
    int          l_iResult  = -1;
    int          l_iLen     = 0;
    ns_msg       l_stMsg    = {0};
    ns_rr        l_stRr     = {0};
    tUINT16      l_sMsgCount= 0;

    l_iLen = res_query(i_pDomain, ns_c_in, ns_t_txt, l_pTxtBuf, (int)l_szTxtBuf);
    if (l_iLen < 0)
    {
        return FALSE;    
    }
    
    l_iResult = ns_initparse(l_pTxtBuf, l_iLen, &l_stMsg);
    if (l_iResult < 0)
    {
        return FALSE;    
    }
   
    l_sMsgCount = ns_msg_count(l_stMsg, ns_s_an);
    for (tUINT16 i = 0; i < l_sMsgCount; i++)
    {
        if (    (0 == ns_parserr(&l_stMsg, ns_s_an, i, &l_stRr))
             && (l_stRr.rdlength)
            )
        {
            tACHAR *l_pText = (tACHAR*)malloc(l_stRr.rdlength-1);
            if (l_pText)
            {
                //TXT record RDATA format is:
                // length, 1 byte
                // string, <length bytes>
                // length2, 1 byte
                // string2, <length2 bytes>
                // etc.            
                memcpy(l_pText, (tACHAR *)l_stRr.rdata + 1, l_stRr.rdlength - 1);
                l_pText[l_stRr.rdlength - 1] = 0;
                io_rList.Push_Last(l_pText);
            }
        }
    }
    
    return (io_rList.Count() != 0);
}
