/*
 Copyright (c) 2017-2018 (c) Project "DeM Labs Inc" https://github.com/demlabsinc
  All rights reserved.

 This file is part of DAP (Deus Applications Prototypes) the open source project

    DAP (Deus Applicaions Prototypes) is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    DAP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with any DAP based project.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include "dap_common.h"
#include "stream_session.h"

#define LOG_TAG "stream_session"

stream_session_t * sessions=NULL;

int stream_session_close2(stream_session_t * s);
static void * session_check(void * data);

void stream_session_init()
{
    log_it(L_INFO,"[session] Init module");
    srand ( time(NULL) );
}

void stream_session_deinit()
{
    stream_session_t *current, *tmp;
    log_it(L_INFO,"[session] Destroy everything");

      HASH_ITER(hh, sessions, current, tmp) {
          HASH_DEL(sessions,current);
          stream_session_close2(current);
      }
}

static void * session_check(void * data)
{
    return NULL;
}


stream_session_t * stream_session_pure_new()
{
    stream_session_t * ret=NULL;
    unsigned int session_id=0,session_id_new=0;
    do{
        session_id_new=session_id=rand()+rand()*0x100+rand()*0x10000+rand()*0x01000000;
        HASH_FIND_INT(sessions,&session_id_new,ret);
    }while(ret);
    log_it(L_INFO,"[session] Creating new with id %u",session_id);
    ret=(stream_session_t*) calloc(1,sizeof(stream_session_t));
    pthread_mutex_init(&ret->mutex, NULL);
    ret->id=session_id;
    ret->time_created=time(NULL);
    ret->create_empty=true;
    ret->enc_type = 0x01; // Default encryption type
    log_it(L_DEBUG,"[session] timestamp %u",(unsigned int) ret->time_created);
    HASH_ADD_INT(sessions,id,ret);
    return ret;
}

stream_session_t * stream_session_new(unsigned int media_id, bool open_preview)
{
    stream_session_t * ret=stream_session_pure_new();
    ret->media_id=media_id;
    ret->open_preview=open_preview;
    ret->create_empty=false;
    return ret;
}

stream_session_t * stream_session_id(unsigned int id)
{
    stream_session_t * ret;
    HASH_FIND_INT(sessions,&id,ret);
    return ret;
}


int stream_session_close(unsigned int id)
{
    return stream_session_close2(stream_session_id(id));
}

int stream_session_close2(stream_session_t * s)
{
    log_it(L_INFO,"[session] Close");
    HASH_DEL(sessions,s);
    free(s);
    return 0;
}

int stream_session_open(stream_session_t * ss)
{
    int ret;
    pthread_mutex_lock(&ss->mutex);
    ret=ss->opened;
    if(ss->opened==0) ss->opened=1;
    pthread_mutex_unlock(&ss->mutex);
    return ret;
}
