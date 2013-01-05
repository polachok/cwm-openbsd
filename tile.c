/*
 * calmwm - the calm window manager
 *
 * Copyright (c) 2004 Marius Aamodt Eriksen <marius@monkey.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * $OpenBSD$
 */

#include <sys/param.h>
#include <sys/queue.h>

#include "calmwm.h"

void
tile_horiz(struct client_ctx *cc)
{
	struct client_ctx	*ci;
	struct group_ctx 	*gc = cc->group;
	struct screen_ctx 	*sc = cc->sc;
	struct geom 		*sa = &sc->work;
	int 			 i, n, mh, x, h, w;
	int 			 pstate;

	if (!gc)
		return;
	i = n = 0;

	pstate = gc->state;
	gc->state = STATE_TILING;
	TAILQ_FOREACH(ci, &gc->clients, group_entry) {
		if (ci->flags & CLIENT_HIDDEN ||
		    ci->flags & CLIENT_IGNORE || (ci == cc))
			continue;
		n++;
	}
	if (n == 0)
		return;

	if (cc->flags & CLIENT_VMAXIMIZED ||
		       	cc->geom.h + 2*cc->bwidth >= sc->work.h)
		return;

	if (pstate == STATE_FLOATING)
		cc->savegeom = cc->geom;
	cc->flags &= ~CLIENT_HMAXIMIZED;
	cc->geom.y = sc->work.y;
	cc->geom.x = sc->work.x;
	cc->geom.w = sc->work.w;
	client_resize(cc, 1);
	client_ptrwarp(cc);

	mh = cc->geom.h + 2 * cc->bwidth;
	x = sa->x;
	w = sa->w/n;
	h = sa->h - mh;
	TAILQ_FOREACH(ci, &gc->clients, group_entry) {
		if (ci->flags & CLIENT_HIDDEN ||
		    ci->flags & CLIENT_IGNORE || (ci == cc))
			continue;
		if (pstate == STATE_FLOATING)
			ci->savegeom = ci->geom;
		ci->bwidth = Conf.bwidth;
		ci->geom.y = sa->y + mh;
		ci->geom.x = x;
		ci->geom.h = h - 2*ci->bwidth;
		ci->geom.w = w - 2*ci->bwidth;
		if (i+1 == n)
			ci->geom.w = sa->x + sa->w - ci->geom.x - 2*ci->bwidth;
		x += w;
		client_resize(ci, 1);
		i++;
	}
}

void
tile_vert(struct client_ctx *cc)
{
	struct client_ctx	*ci;
	struct group_ctx 	*gc = cc->group;
	struct screen_ctx 	*sc = cc->sc;
	struct geom 		*sa = &sc->work;
	int 			 i, n, mw, y, h, w;
	int 			 pstate;

	if (!gc)
		return;

	i = n = 0;

	pstate = gc->state;
	gc->state = STATE_TILING;
	TAILQ_FOREACH(ci, &gc->clients, group_entry) {
		if (ci->flags & CLIENT_HIDDEN ||
		    ci->flags & CLIENT_IGNORE || (ci == cc))
			continue;
		n++;
	}
	if (n == 0)
		return;

	if (cc->flags & CLIENT_HMAXIMIZED ||
			cc->geom.w + 2*cc->bwidth >= sc->work.w)
		return;

	if (pstate == STATE_FLOATING)
		cc->savegeom = cc->geom;
	cc->flags &= ~CLIENT_VMAXIMIZED;
	cc->geom.x = sc->work.x;
	cc->geom.y = sc->work.y;
	cc->geom.h = sc->work.h - 2 * cc->bwidth;
	client_resize(cc, 1);
	client_ptrwarp(cc);

	mw = cc->geom.w + 2 * cc->bwidth;
	y = sa->y;
	h = sa->h/n;
	w = sa->w - mw;
	TAILQ_FOREACH(ci, &gc->clients, group_entry) {
		if (ci->flags & CLIENT_HIDDEN ||
		    ci->flags & CLIENT_IGNORE || (ci == cc))
			continue;
		if (pstate == STATE_FLOATING)
			ci->savegeom = ci->geom;
		ci->bwidth = Conf.bwidth;
		ci->geom.y = y;
		ci->geom.x = sa->x + mw;
		ci->geom.h = h - 2*ci->bwidth;
		ci->geom.w = w - 2*ci->bwidth;
		if (i+1 == n)
			ci->geom.h = sa->y + sa->h - ci->geom.y - 2*ci->bwidth;
		y += h;
		client_resize(ci, 1);
		i++;
	}
}

void
tile_untile(struct client_ctx *cc)
{
	struct client_ctx	*ci;
	struct group_ctx 	*gc = cc->group;

	if (!gc || gc->state == STATE_FLOATING)
		return;
	gc->state = STATE_FLOATING;

	TAILQ_FOREACH(ci, &gc->clients, group_entry) {
		if (ci->flags & CLIENT_HIDDEN ||
		    ci->flags & CLIENT_IGNORE)
			continue;
		kbfunc_undo(ci, 0);
	}
}
