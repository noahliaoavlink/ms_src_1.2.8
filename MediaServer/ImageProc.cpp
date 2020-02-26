#include "stdafx.h"
#include "Allocator.h"

//#include "MyEffect.h"
 
 
void CAllocator::VideoEffect( int effectID, struct sEffectD3DParam D3DParam,  struct sEffectParam EffectParam )
{
	switch(effectID)
	{
	case 0: m_ImgEffect.NormalVideo(D3DParam, EffectParam );				break;
	case 1: m_ImgEffect3D.Negative( D3DParam, EffectParam );				break;
	case 2: m_ImgEffect3D.Gray( D3DParam, EffectParam );					break;
	case 3: m_ImgEffect3D.RGBColor( D3DParam, EffectParam);				break;
	//case 4:	m_ImgEffect.Brightness( D3DParam, EffectParam );				break;
	case 4:	m_ImgEffect3D.BrightnessFlicker( D3DParam, EffectParam );		break;
	case 5:	m_ImgEffect.SleazyEmboss( D3DParam, EffectParam );			break;
	case 6:	m_ImgEffect3D.RectangleBrightDark( D3DParam, EffectParam );	break;
	case 7: m_ImgEffect.DelayMovie(D3DParam, EffectParam);				break;
	case 8: m_ImgEffect3D.MultiMovie( D3DParam, EffectParam );			break;
	case 9: m_ImgEffect3D.Pixelate( D3DParam, EffectParam );				break;
	case 10: m_ImgEffect3D.RotateScale( D3DParam, EffectParam );			break;
	//case 12: m_ImgEffect.ColorConvert( D3DParam, EffectParam );			break;
	case 11: m_ImgEffect3D.Crosshatching( D3DParam, EffectParam );			break;
	case 12: m_ImgEffect3D.SwirlTexture( D3DParam, EffectParam );		break;
	case 13: m_ImgEffect3D.BlindTexture(D3DParam, EffectParam);			break;
	case 14: m_ImgEffect3D.NeonLightTexture(D3DParam, EffectParam);		break;
	case 15: m_ImgEffect3D.HSL_RGB(D3DParam, EffectParam);				break;
	case 16: m_ImgEffect3D.WindowsNormal(D3DParam, EffectParam);			break;
	case 17: m_ImgEffect3D.Ripple(D3DParam, EffectParam);				break;
	case 18: m_ImgEffect3D.FishEye(D3DParam, EffectParam);				break;
	case 19: m_ImgEffect3D.Halftone(D3DParam, EffectParam);				break;
	case 20: m_ImgEffect3D.WoodNoise(D3DParam, EffectParam);				break;
	case 21: m_ImgEffect3D.Edge(D3DParam, EffectParam);					break;
	case 22: m_ImgEffect3D.Wave(D3DParam, EffectParam);					break;
	case 23: m_ImgEffect3D.WaterColor(D3DParam, EffectParam);			break;
	case 24: m_ImgEffect3D.Kaleidoscope(D3DParam, EffectParam);			break;
	case 25: m_ImgEffect3D.CircleCenter(D3DParam, EffectParam);			break;
	case 26: m_ImgEffect3D.CylinderRotate(D3DParam, EffectParam);		break;
	case 27: m_ImgEffect3D.PlaneRotate(D3DParam, EffectParam);			break;
	case 28: m_ImgEffect3D.DotMosaics(D3DParam, EffectParam);			break;
	case 29: m_ImgEffect3D.SingleColor(D3DParam, EffectParam);			break;
	case 30: m_ImgEffect3D.Blur(D3DParam, EffectParam);			        break;
	case 31: m_ImgEffect3D.Mirage(D3DParam, EffectParam);			    break;
	default:		LOG("Error");		break;
	}

}
