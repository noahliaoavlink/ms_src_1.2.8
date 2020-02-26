#ifndef _EffectParam_H_
#define _EffectParam_H_

typedef struct
{
	char szEffectName[128];
	char szParam[6][128];
	int iID;
}EffectParam;

static EffectParam g_effect_param_items[] =
{
	{ "No Effect","","","","","","" ,0},
	{ "Negative","Level", "","","","","",1 },
	{ "Gray","Level","Contrast","","","","",2 },
	{ "RGB Color","Level", "Red Shift","Green Shift","Blue Shift","","" ,3},
	{ "Brightness Flicker","Level","Frequency","Amplitude","","","" ,4},
//	{ "Sleazy Emboss","Level", "Contast","","","","" ,5},   //
	{ "Rectangle Bright Dark","Level", "Bright Shift","Left","Right","Top", "Down" ,6},
	{ "Freeze Movie","Level","", "","","","" ,7},
//	{ "Multi Movie","Level","Multi", "H Shift","V Shift","","" ,8},   //
	{ "Pixelate","Level","", "","","","" ,9},
	{ "Rotate Scale","Level","Width Zoom", "Heigth Zoom","H Shfit","V Shfit","" ,10},
	{ "Cross Hatching","Level", "Density","","","","" ,11},
	{ "Swirl Texture","Level","Radius", "","","","" ,12},
	{ "Blind Texture","Level", "Density","Contrast","","","" ,13},
	{ "Neon Light Texture","Level", "Light Size","H Move","V Move","","" ,14},
//	{ "Color Modify","Level", "Brightness","Hue","Saturation","","" ,15},
	{ "Glass Windows","Level", "Pattern","","","","" ,16},
	{ "Ripple","Level","Speed","Density","H Shift","V Shift","" ,17},
//	{ "Fish Eye","Level","Layer", "","","","" ,18},
	{ "Halftone","Level","Saturation", "","","","" ,19},
	{ "Wood Noise","Level","G","B","H Num","V Num","" ,20},
//	{ "Edge","Level","", "","","","" ,21},
//	{ "Wave","Level","Frequency","Twist","","","" ,22},
//	{ "WaterColor","Level", "Saturation","","","","" ,23},
	{ "Kaleidoscope","Level", "Speed","","","","" ,24},
	{ "Circle Center","Level","Angle1","Rotate1","Angle2","Rotate2","" ,25},
//	{ "Cylinder Rotate","Level", "Rotate","Shift","","","" ,26},
//	{ "Plane Rotate","Level","Rotate","X Move","Z Move","Distance","" ,27},
//	{ "Dot Mosaics","Level","Brigtness","Soft","","","" ,28},
	{ "Single Color","Level","Red","Green","Blue","Saturation","" ,29},
//	{ "Blur","Level","","","","","" ,30},
	{ "Mirage","Level","Angle","Range","Amplitude","Y Offset","" ,31}
};

enum HD_Effect_ID
{
	HD_EID_BRIGHTNESS = 101,
	HD_EID_CONTRAST,
	HD_EID_HUE,
	HD_EID_SATURATION,
	HD_EID_R,
	HD_EID_G,
	HD_EID_B,
	HD_EID_RGBA,
};

static EffectParam g_hd_effect_param_items[] =
{
	{ "No Effect","","","","","","" ,0 },
	{ "Brightness","Level","","","","","" ,HD_EID_BRIGHTNESS },
	{ "Contrast","Level","","","","","" ,HD_EID_CONTRAST },
	{ "Saturation","Level","","","","","" ,HD_EID_HUE },
	{ "Hue","Level","","","","","" ,HD_EID_SATURATION },
//	{ "R","Level","","","","","" ,HD_EID_R },
//	{ "G","Level","","","","","" ,HD_EID_G },
//	{ "B","Level","","","","","" ,HD_EID_B },
	{ "RGBA","Level","R","G","B","","" ,HD_EID_RGBA },
};


#endif
