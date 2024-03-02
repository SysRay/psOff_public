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
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CRingBuffer
{
public:
    tUINT8 *m_pBufferHead; 
    tUINT8 *m_pBufferTail; 
    size_t  m_szBuffer;
    size_t  m_szWritten;
    tUINT8 *m_pWrite;
    tUINT8 *m_pRead;
    tUINT64 m_qwMarker;
public:
    CRingBuffer(size_t i_szBuffer)
        : m_pBufferHead(NULL)
        , m_pBufferTail(NULL)
        , m_szBuffer(i_szBuffer)
        , m_szWritten(0)
        , m_pWrite(NULL)
        , m_pRead(NULL)
        , m_qwMarker(0xDEADBEEFBEEFDEADull)
    {
        m_pBufferHead = (tUINT8*)malloc(i_szBuffer + sizeof(m_qwMarker));
        if (m_pBufferHead)
        {
            m_pBufferTail = m_pBufferHead + m_szBuffer; 
            memcpy(m_pBufferTail, &m_qwMarker, sizeof(m_qwMarker));
        }


        m_pWrite = m_pBufferHead;
        m_pRead = m_pBufferHead;
    }

    virtual ~CRingBuffer()
    {
        if (m_pBufferHead)
        {
            if (0 != memcmp(m_pBufferTail, &m_qwMarker, sizeof(m_qwMarker)))
            {
                printf("CRingBuffer has been corrupted!");
            }

            free(m_pBufferHead);
            m_pBufferHead = NULL;
        }

        m_pBufferHead  = NULL; 
        m_pBufferTail  = NULL;
        m_szBuffer     = 0;
        m_pWrite       = NULL;
        m_pRead        = NULL;
        m_szWritten    = 0;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    inline size_t GetFreeSize()
    {
        return m_szBuffer - m_szWritten;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    inline size_t GetSize()
    {
        return m_szBuffer;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    inline size_t GetUsedSize()
    {
        return m_szWritten;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    inline tUINT8* GetReadHead(size_t &o_rSize)
    {
        if (!m_szWritten)
        {
            o_rSize = 0;
            return NULL;
        }


        if (m_pWrite > m_pRead)
        {
            o_rSize = m_pWrite - m_pRead;
        }
        else
        {
            o_rSize = m_pBufferTail - m_pRead;
        }

        return m_pRead;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    inline bool MoveReadHead(size_t o_szIncrease)
    {
        if (o_szIncrease > m_szWritten)
        {
            return false;
        }

        if (m_pWrite > m_pRead)
        {
            m_pRead     += o_szIncrease;
            m_szWritten -= o_szIncrease;
            return true;
        }
        else
        {
            size_t l_szTail = m_pBufferTail - m_pRead;

            if (o_szIncrease > l_szTail)
            {
                return false; //impossible to get continuous buffer consisting of 2 parts :)
            }
            else
            {
                m_szWritten -= o_szIncrease;
                m_pRead     += o_szIncrease;

                if (m_pRead >= m_pBufferTail)
                {
                    m_pRead = m_pBufferHead;
                }

                return true;
            }
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __forceinline bool Write(const tUINT8 *i_pData, size_t i_szData)
    {
        if (i_szData > (m_szBuffer - m_szWritten))
        {
            return false;
        }

        m_szWritten += i_szData;

        if (m_pWrite < m_pRead)
        {
            memcpy(m_pWrite, i_pData, i_szData);
            m_pWrite += i_szData;
        }
        else
        {
            size_t l_szTail = m_pBufferTail - m_pWrite;
            if (i_szData < l_szTail)
            {
                memcpy(m_pWrite, i_pData, i_szData);
                m_pWrite += i_szData;
            }
            else
            {
                memcpy(m_pWrite, i_pData, l_szTail);
                i_pData  += l_szTail;
                i_szData -= l_szTail;

                memcpy(m_pBufferHead, i_pData, i_szData);

                m_pWrite = m_pBufferHead + i_szData; 
            }
        }

        return true;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    inline bool Read(tUINT8 *o_pData, size_t i_szData)
    {
        if (i_szData > GetUsedSize())
        {
            return false;
        }

        m_szWritten -= i_szData;

        if (m_pWrite > m_pRead)
        {
            memcpy(o_pData, m_pRead, i_szData);
            m_pRead += i_szData;
        }
        else
        {
            size_t l_szTail = m_pBufferTail - m_pRead;

            if (i_szData >= l_szTail)
            {
                memcpy(o_pData, m_pRead, l_szTail);
                o_pData  += l_szTail;
                i_szData -= l_szTail;

                memcpy(o_pData, m_pBufferHead, i_szData);

                m_pRead = m_pBufferHead + i_szData;
            }
            else
            {
                memcpy(o_pData, m_pRead, i_szData);
                m_pRead += i_szData;
            }
        }


        if (0 != memcmp(m_pBufferTail, &m_qwMarker, sizeof(m_qwMarker)))
        {
            printf("CRingBuffer has been corrupted!");
        }

        return true;

    }
};


#endif //RING_BUFFER_H
