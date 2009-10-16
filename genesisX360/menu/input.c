#include "libxenon.h"
#include "menu.h"


void guideCalled()
{
	fastPrintf(340,200,0x00FF00,"Guide ...");
}

//Bouton Appuyer
struct controller_data_s last_data_input;
void pushBtn(struct controller_data_s controller_data)
{
	if((controller_data.up)&&(!last_data_input.up))
	{
		iSelectedItem--;
		xe_framecount=0;//remise a 0
		sound_select();

	}
	if((controller_data.down)&&(!last_data_input.down))
	{
		iSelectedItem++;
		xe_framecount=0;//remise a 0
		sound_select();
	}
	if((controller_data.a)&&(!last_data_input.a))
	{
		HandleAction();
		xe_framecount=0;//remise a 0
	}
	if((controller_data.select)&&(!last_data_input.select))
	{
		//guideCalled();
		shutdown();
	}
	if((controller_data.rb)&&(!last_data_input.rb))
	{
		//guideCalled();
		iSelectedItem+=nItemOnScreen;
		xe_framecount=0;//remise a 0
		sound_select();
	}
	if((controller_data.rt))
	{
		//guideCalled();
		iSelectedItem+=nItemOnScreen;
		xe_framecount=0;//remise a 0
		sound_select();
	}

	if((controller_data.lb)&&(!last_data_input.lb))
	{
		//guideCalled();
		iSelectedItem-=nItemOnScreen;
		xe_framecount=0;//remise a 0
		sound_select();
	}
	if((controller_data.lt))
	{
		//guideCalled();
		iSelectedItem-=nItemOnScreen;
		xe_framecount=0;//remise a 0
		sound_select();
	}

	//Mets à jour la valeur
	if(iSelectedItem<0)
		iSelectedItem=0;
	if(iSelectedItem>=maxItem)
		iSelectedItem=maxItem-1;

	struct controller_data_s c;
	c=controller_data;

	last_data_input=controller_data;
}

