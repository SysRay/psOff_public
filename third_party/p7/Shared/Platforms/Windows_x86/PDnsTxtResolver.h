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
#include <Windns.h>
#include <time.h>

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
    DNS_RECORDA *l_pRecord    = NULL;
    DNS_RECORDA *l_pIter      = NULL;
    DNS_STATUS   l_lStatus    = 0;
    tACHAR      *l_pReturn    = NULL;

    io_rList.Clear(TRUE);

    l_lStatus = DnsQuery_A(i_pDomain, 
                           DNS_TYPE_TEXT, 
                             DNS_QUERY_STANDARD
                           | DNS_QUERY_WIRE_ONLY
                           | DNS_QUERY_NO_LOCAL_NAME
                           | DNS_QUERY_BYPASS_CACHE
                           //| DNS_QUERY_USE_TCP_ONLY
                           | DNS_QUERY_NO_HOSTS_FILE,
                           NULL,
                           (DNS_RECORD**)&l_pRecord,
                           NULL
                          );
    if (    (0 == l_lStatus)
         && (l_pRecord)
       )
    {
        l_pIter = l_pRecord;
        while (l_pIter)  
        {
            if (DNS_TYPE_TEXT == l_pIter->wType)
            {
                if (l_pIter->Data.Txt.dwStringCount)
                {
                    for (DWORD l_dwI = 0; l_dwI < l_pIter->Data.Txt.dwStringCount; l_dwI++)
                    {
                        if (l_pIter->Data.Txt.pStringArray[l_dwI])
                        {
                            io_rList.Push_Last(strdup(l_pIter->Data.Txt.pStringArray[l_dwI]));
                        }
                    }
                }
            }

            if (!l_pReturn)
            {
                l_pIter = l_pIter->pNext;
            }
            else
            {
                l_pIter = NULL;
            }
        }

        DnsRecordListFree(l_pRecord, DnsFreeRecordList);
    }

    return (io_rList.Count() != 0);
}
