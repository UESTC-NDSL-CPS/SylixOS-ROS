/**
 * @file
 * SylixOS multi-input device support kernel module.
 */

/*
 * Copyright (c) 2006-2022 SylixOS Group.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 * 4. This code has been or is applying for intellectual property protection
 *    and can only be used with acoinfo software products.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * Author: Han.hui <sylixos@gmail.com>
 *
 */

#ifndef __XINPUT_LIST_H
#define __XINPUT_LIST_H

/* List initialize */
#define INIT_LIST(node) \
        (node)->next = (node)->prev = LW_NULL;

/* List add / delete */
#define INSERT_TO_HEADER(node, header) \
        do { \
            (node)->next = (header); \
            (node)->prev = LW_NULL; \
            if (header) { \
                (header)->prev = (node); \
            } \
            (header) = (node); \
        } while (0)

#define DELETE_FROM_LIST(node, header) \
        do { \
            if (!(node)->prev) { \
                (header) = (node)->next; \
            } else { \
                (node)->prev->next = (node)->next; \
            } \
            if ((node)->next) { \
                (node)->next->prev = (node)->prev; \
            } \
        } while (0)

/* List add / delete with void header */
#define INSERT_TO_HEADER_VOID(node, header, type) \
        do { \
            (node)->next = (type *)(header); \
            (node)->prev = LW_NULL; \
            if (header) { \
                ((type *)(header))->prev = (node); \
            } \
            (header) = (void *)(node); \
        } while (0)

#define DELETE_FROM_LIST_VOID(node, header) \
        do { \
            if (!(node)->prev) { \
                (header) = (void *)(node)->next; \
            } else { \
                (node)->prev->next = (node)->next; \
            } \
            if ((node)->next) { \
                (node)->next->prev = (node)->prev; \
            } \
        } while (0)

/* List concat */
#define CONCAT_TO_HEADER(type, header_d, header_s) \
        do { \
            if (header_d) { \
                type *tail; \
                for (tail = (header_d); tail->next; tail = tail->next); \
                tail->next = (header_s); \
                if (header_s) { \
                    (header_s)->prev = (tail); \
                } \
            } else { \
                (header_d) = (header_s); \
            } \
        } while (0)

/* List walk */
#define FOREACH_FROM_LIST(node, header) \
        for ((node) = (header); (node) != LW_NULL; (node) = (node)->next)

#define FOREACH_FROM_LIST_REVERSE(node, tail) \
        for ((node) = (tail); (node) != LW_NULL; (node) = (node)->prev)

#define FOREACH_FROM_LIST_SAFE(node, n, header) \
        for ((node) = (header), (n) = (node) ? (node)->next : LW_NULL; \
             (node) != LW_NULL; (node) = (n), (n) = (n) ? (n)->next : LW_NULL)

/* FIFO List operate */
#define INSERT_TO_FIFO(node, header, tail) \
        do { \
            (node)->next = LW_NULL; \
            (node)->prev = tail; \
            if (tail) { \
                (tail)->next = (node); \
            } \
            (tail) = (node); \
            if (!(header)) { \
                (header) = (node); \
            } \
        } while (0)

#define DELETE_FROM_FIFO(node, header, tail) \
        do { \
            if (!(node)->prev) { \
                (header) = (node)->next; \
            } \
            if (!(node)->next) { \
                (tail) = (node)->prev; \
            } \
            if ((node)->prev) { \
                (node)->prev->next = (node)->next; \
            } \
            if ((node)->next) { \
                (node)->next->prev = (node)->prev; \
            } \
        } while (0)

/* Insert left and right */
#define INSERT_TO_LEFT(node, right, header) \
        do { \
            (node)->next = (right); \
            (node)->prev = (right)->prev; \
            if ((right)->prev) { \
                (right)->prev->next = (node); \
            } \
            (right)->prev = (node); \
            if ((header) == (right)) { \
                (header) = (node); \
            } \
        } while (0)

#define INSERT_TO_RIGHT(node, left, tail) \
        do { \
            (node)->prev = (left); \
            (node)->next = (left)->next; \
            if ((left)->next) { \
                (left)->next->prev = (node); \
            } \
            (left)->next = (node); \
            if ((tail) == (left)) { \
                (tail) = (node); \
            } \
        } while (0)

#endif /* __XINPUT_LIST_H */
/*
 * end
 */
