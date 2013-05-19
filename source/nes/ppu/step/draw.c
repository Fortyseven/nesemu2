/***************************************************************************
 *   Copyright (C) 2013 by James Holodnak                                  *
 *   jamesholodnak@gmail.com                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

static INLINE void drawpixel()
{
	u8 *dest = nes.ppu.linebuffer;
	u8 *src = nes.ppu.tilebuffer;
	int pos = LINECYCLES - 1;

	if(pos >= 8 || (nes.ppu.control1 & 2))
		dest[pos] = src[pos + nes.ppu.scrollx];
	else
		dest[pos] = 0;
	//color emphasis
#ifdef ACCURATE_SPRITE0
	sprite0_hit_check();
#endif
}

static INLINE void drawpixelclipped()
{
	u8 *dest = nes.ppu.linebuffer;
	u8 *src = nes.ppu.tilebuffer;
	int pos = LINECYCLES - 1;

	dest[pos] = src[pos + nes.ppu.scrollx];
}

static INLINE void drawspriteline()
{
	static u8 spriteline[256 + 8];
	sprtemp_t *spr = (sprtemp_t*)sprtemp + 7;
	u64 *spriteline64 = (u64*)spriteline;
	u8 *dest = nes.ppu.linebuffer;
	int n;

	//clear sprite line
	for(n=0;n<(256 / 8);n++)
		spriteline64[n] = 0x00;

	//loop thru all eight possible sprites
	for(n=0;n<8;n++,spr--) {
		cache_t offs;
		u64 *scr64,sp0,sp1,pmask0,pmask1,pmaskxor = 0xFFFFFFFFFFFFFFFFLL;
		u64 shiftright = (8 - (spr->x & 7)) * 8;
		u64 shiftleft = (spr->x & 7) * 8;

		if(spr->flags == 0)
			continue;

		//get offset in sprite buffer
		scr64 = ((u64*)spriteline) + (spr->x / 8);

		//setup to draw the pixel
		offs = spr->attr * 0x0404040404040404LL;
		sp0 = sp1 = spr->line;
		pmask0 = pmask1 = (0x8080808080808080LL - sp0) >> 2;

#ifndef ACCURATE_SPRITE0
		//sprite is sprite 0, check for hit
		if(spr->flags & 2) {
			u64 sprtileline;
			u8 *sprtileline8,*bg;
			int j,x;

			sprtileline = sp0 & pmask0;
			sprtileline8 = ((u8*)&sprtileline);
			bg = dest + spr->x;
			for(x=spr->x,j=0;j<8 && (spr->x + j) < 255;j++,x++) {
				if(((CONTROL1 & 4) == 0) && (x < 8))
					continue;
				if(sprtileline8[j] && ((bg[j] & 0xF) != 0)) {
					STATUS |= 0x40;
					spr->flags = 0;
					break;
				}
			}
		}
#endif

		//render pixel to sprite line buffer
#ifdef __GNUC__
		if(spr->x & 7) {
#endif
			sp0 <<= shiftleft;
			sp1 >>= shiftright;
			pmask0 <<= shiftleft;
			pmask1 >>= shiftright;
			scr64[0] = ((sp0 + offs) & pmask0) | (scr64[0] & ~pmask0);
			scr64[1] = ((sp1 + offs) & pmask1) | (scr64[1] & ~pmask1);
#ifdef __GNUC__
		}
		else
			scr64[0] = ((sp0 + offs) & pmask0) | (scr64[0] & ~pmask0);
#endif
	}

	//draw the sprite line to the buffer
	for(n=(((CONTROL1 ^ 4) & 4) << 1);n<256;n++) {
		u8 pixel = spriteline[n];

		if(pixel & 3) {
			if((pixel & 0x10) == 0)    //foreground sprite
				dest[n] = pixel | 0x10;
			else if((dest[n] & 3) == 0)      //background sprite that is visible
				dest[n] = pixel | 0x10;
		}
	}
}

static INLINE void update_line()
{
#ifdef QUICK_SPRITES
	if(CONTROL1 & 0x10)
		drawspriteline();
#endif
	video_updateline(SCANLINE,nes.ppu.linebuffer);
}
