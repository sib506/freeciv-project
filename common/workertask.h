/********************************************************************** 
 Freeciv - Copyright (C) 1996 - A Kjeldberg, L Gregersen, P Unold
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/
#ifndef FC__WORKERTASK_H
#define FC__WORKERTASK_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct worker_task
{
  struct tile *ptile;
  enum unit_activity act;
  struct act_tgt tgt;
  int want;
};

/* So can use between load/ save games*/
struct worker_task_load_compatible
{
  int ptile_x;
  int ptile_y;
  enum unit_activity act;
  struct act_tgt tgt;
};

void worker_task_init(struct worker_task *ptask);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* FC__WORKERTASK_H */
