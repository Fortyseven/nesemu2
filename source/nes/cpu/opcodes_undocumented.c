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

//undocumented opcodes
static INLINE void OP_LAX()
{
	A = X = memread(EFFADDR);
	checknz(A);
}

static INLINE void OP_SAX()
{
	memwrite(EFFADDR,A & X);
}

static INLINE void OP_DCP()
{
	tmp8 = memread(EFFADDR);
	memwrite(EFFADDR,tmp8--);
	memwrite(EFFADDR,tmp8);
	tmpi = A - tmp8;
	FLAG_C = (tmpi >= 0) ? 1 : 0;
	checknz((u8)tmpi);
}

static INLINE void OP_ISB()
{
	tmp8 = memread(EFFADDR);
	memwrite(EFFADDR,tmp8++);
	memwrite(EFFADDR,tmp8);
	tmpi = A - tmp8 - (1 - FLAG_C);
	FLAG_C = ((tmpi & 0xFF00) == 0) ? 1 : 0;
	FLAG_V = (((A ^ tmp8) & (A ^ tmpi)) & 0x80) ? 1 : 0;
	A = (u8)tmpi;
	checknz(A);
}

static INLINE void OP_SLO()
{
	TMPREG = memread(EFFADDR);
	memwrite(EFFADDR,TMPREG);
	FLAG_C = (TMPREG >> 7) & 1;
	TMPREG <<= 1;
	A |= TMPREG;
	memwrite(EFFADDR,TMPREG);
	checknz(A);
}

static INLINE void OP_RLA()
{
	TMPREG = memread(EFFADDR);
	memwrite(EFFADDR,TMPREG);
	tmp8 = FLAG_C;
	FLAG_C = (TMPREG >> 7) & 1;
	TMPREG = (TMPREG << 1) | tmp8;
	memwrite(EFFADDR,TMPREG);
	A &= TMPREG;
	checknz(A);
}

static INLINE void OP_SRE()
{
	TMPREG = memread(EFFADDR);
	memwrite(EFFADDR,TMPREG);
	FLAG_C = TMPREG & 1;
	TMPREG >>= 1;
	memwrite(EFFADDR,TMPREG);
	A ^= TMPREG;
	checknz(A);
}

static INLINE void OP_RRA()
{
	TMPREG = memread(EFFADDR);
	memwrite(EFFADDR,TMPREG);
	tmp8 = FLAG_C;
	FLAG_C = TMPREG & 1;
	TMPREG = (TMPREG >> 1) | (tmp8 << 7);
	memwrite(EFFADDR,TMPREG);
	tmpi = A + TMPREG + FLAG_C;
	FLAG_C = (tmpi & 0xFF00) ? 1 : 0;
	FLAG_V = (((A ^ tmpi) & (TMPREG ^ tmpi)) & 0x80) ? 1 : 0;
	A = (u8)tmpi;
	checknz(A);
}

//"strange behavior" opcodes
static INLINE void OP_AAC()
{
	A &= memread(EFFADDR);
	checknz(A);
	FLAG_C = FLAG_N;
}

static INLINE void OP_ASR()
{
	A &= memread(EFFADDR);
	FLAG_C = A & 1;
	A >>= 1;
	checknz(A);
}

static INLINE void OP_ARR()
{
	A &= memread(EFFADDR);
	A = (A >> 1) | (FLAG_C << 7);
	checknz(A);
	FLAG_C = (A >> 6) & 1;
	FLAG_V = ((A >> 5) & 1) ^ FLAG_C;
}

static INLINE void OP_ATX()
{
//	A &= memread(EFFADDR);
	A = memread(EFFADDR);
	X = A;
	checknz(A);
}

static INLINE void OP_AXS()
{
	tmpi = (X & A) - memread(EFFADDR);
	X = (u8)tmpi;
	checknz(X);
	if(tmpi >= 0)
		FLAG_C = 1;
	else
		FLAG_C = 0;
}
/*
// 9E: // shx $1234,y
Array
(
[1005] => addr = PB();
[1020] => d = Y;
[1050] => addr = u8(addr) + 256 * PB();
[1080] => RB(wrap(addr, addr+d));
[1262] => WB(wrap(addr, addr+d), X & ((addr+d) >> 8));
)

// 9C: // shy $1234,x
Array
(
[1005] => addr = PB();
[1010] => d = X;
[1050] => addr = u8(addr) + 256 * PB();
[1080] => RB(wrap(addr, addr+d));
[1263] => WB(wrap(addr, addr+d), Y & ((addr+d) >> 8));
)

Where PB() is equal to RB(PC++),
RB(addr) reads a byte from given address,
and WB(addr, value) writes a byte into the given address;
and wrap(addr, addr2) is equal to (addr & 0xFF00) + (addr2 & 0xFF). 
*/
static INLINE void OP_SYA()
{
	tmp8 = Y & ((EFFADDR >> 8) + 1);
	memread(EFFADDR);
	memwrite(EFFADDR,tmp8);

}

static INLINE void OP_SXA()
{
	tmp8 = X & ((EFFADDR >> 8) + 1);
	memread(EFFADDR);
	memwrite(EFFADDR,tmp8);

}
