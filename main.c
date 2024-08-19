#include "base.h"


#include "glider_gun.h"
//hi

void smtren(word addr)
{
    word i = 0;
    word j = 0;
    for(i = 0; i < 0x0600; i++)
    {
        j++;
        if((j & 0x001F) == 0x18)
        {
            j+=8;
        }
        *((word *)(0x9000 + j)) = *((word *)(addr + i));
    }
}

void samtren(word addr)
{
    word i = 0;
    word j = 0;
    for(i = 0; i < 0x0600; i++)
    {
        j++;
        if((j & 0x001F) == 0x18)
        {
            j+=8;
        }
        *((word *)(0x9800 + j)) = *((word *)(addr + i));
    }
}

void delay(ushort after_ticks)
{
	if ((FCON & 2) != 0)
		FCON &= 0xfd;
	__DI();
	Timer0Interval = after_ticks;
	Timer0Counter = 0;
	Timer0Control = 0x0101;
	InterruptPending_W0 = 0;
	StopAcceptor = 0x50;
	StopAcceptor = 0xa0;
	StopControl = 2;
	__asm("nop");
	__asm("nop");
	__EI();
}

void set_pixel(word x, word y,byte color) {
	word addr = (y<<5) + (x >> 3) + 0xF800;
	byte ty = 0x80>>(x & 7);
	if(color)
	{
		val(addr) |= ty;
		return;
	}
	val(addr) &= ~ty;
}


void set_pixel_buf(word x, word y,byte color) {
	word addr = (y<<5) + (x >> 3) + 0x9800;
	byte ty = 0x80>>(x & 7);
	if(color)
	{
		val(addr) |= ty;
		return;
	}
	val(addr) &= ~ty;
}


enum SPECIAL_CHARS
{
	SP_EXE = '\n',
	SP_TAB = '\t',
	SP_SPACE = ' ',
	SP_BACKSPACE = '\b',

	SP_HOME = 0x2A,
	SP_END = 0x1A,
	SP_YES = 0x3A,
	SP_NO = 0x12,
	SP_OK = 0x21,
	SP_UP = 0x20,
	SP_DOWN = 0x22,
	SP_LEFT = 0x29,
	SP_RIGHT = 0x19,
	SP_PLUS = 0x10,
	SP_MINUS = 0x11,
	SP_ALT = 0x31,
	SP_ABC = 0x30,
	SP_ESC = 0x39,
	SP_SELECTLEFT,
	SP_SELECTRIGHT,
	SP_PASTE,
	SP_COPY,

	SPECIAL_MAX
};

byte lastbutton = 0xff;
byte CheckButtons()
{
	byte x;
	byte y;
	byte i = 0;
	for(x = 0x80; x != 0; x = x >> 1)
	{
		deref(0xf046) = x;
		for(y = 0x80; y != 0; y = y >> 1)
		{
			if((deref(0xf040) & y) == 0)
			{
				if(i != lastbutton)
				{
					lastbutton = i;
					return i;
				}
				return 0xff;
			}
			++i;
		}
	}
	lastbutton = 0x50;
	return 0xff;
}


void render()
{
	word i = 0;
	for(i=0;i<2048;i++)
	{
		deref(0x9000+i) = deref(0x9800+i);
		deref(0xf037) = 0;
		deref(0xf800+i) = deref(0x9000+i);
		deref(0xf037) = 4;
		deref(0xf800+i) = deref(0x9000+i);
	}
}

void main() {
	word i = 0;
	word dl = 0x201;
	word j = 0;
	byte curr = 0;
	byte sum = 0;
	deref(0xf034) = 0x06;
	deref(0xf036) = 0x08;

	for(i=0;i<2048;i++)
	{
		deref(0xf037) = 0;
		deref(0xf800+i) = 0;
		deref(0xf037) = 4;
		deref(0xf800+i) = 0;
	}
	for(i=0;i<2048;i++)
	{
		deref(0x9000+i) = deref(*gg+i);
		deref(0x9800+i) = deref(*gg+i);
	}
	/*
	while(1)
	{
		deref(0xf037) = 0;
		samtren(p11);
		deref(0xf037) = 4;
		samtren(p12);

		delay(dl);

		deref(0xf037) = 0;
		samtren(p21);
		deref(0xf037) = 4;
		samtren(p22);

		delay(dl);

		deref(0xf037) = 0;
		samtren(p31);
		deref(0xf037) = 4;
		samtren(p32);

		delay(dl);

		deref(0xf037) = 0;
		samtren(p41);
		deref(0xf037) = 4;
		samtren(p42);

		delay(dl);

		deref(0xf037) = 0;
		samtren(p51);
		deref(0xf037) = 4;
		samtren(p52);

		delay(dl);

		deref(0xf037) = 0;
		samtren(p61);
		deref(0xf037) = 4;
		samtren(p62);

		delay(dl);

	}
	*/
	/*
	smtren(dark);
	deref(0xf037) = 0;
	samtren(dark);
	deref(0xf037) = 4;
	samtren(dark);
	*/
	while(1)
	{
		for(i=0;i<192;i++)
		{
			for(j=1;j<63;j++)
			{
				curr = (val((j<<5)+(i>>3)+0x9000)&(0x80>>(i&7)))?1:0;
				sum = get_pixel(i,j-1) + get_pixel(i+1,j-1) + get_pixel(i+1,j) + get_pixel(i+1,j+1) + get_pixel(i,j+1) + get_pixel(i-1,j+1) + get_pixel(i-1,j) + get_pixel(i-1,j-1);
				if(curr==1)
				{
					if(sum==2||sum==3)
					{
						set_pixel_buf(i,j,1);
						continue;
					}
					set_pixel_buf(i,j,0);
					continue;
				}
				if(sum==3)
				{
					set_pixel_buf(i,j,1);
				}
			}
		}
		render();
	}


}

byte get_pixel(word x, word y)
{

    return(val((y<<5)+(x>>3)+0x9000)&(0x80>>(x&7)))?1:0;
}

/*
void plot_line (int x0, int y0, int x1, int y1)
{
  int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy, e2;

  for (;;){
    set_pixel (x0,y0);
    if (x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; }
    if (e2 <= dx) { err += dx; y0 += sy; }
  }
}
*/
void custom_break()
{
	while(1)
	{

	}
}
