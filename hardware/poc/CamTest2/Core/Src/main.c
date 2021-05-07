/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
DMA_HandleTypeDef hdma_tim1_ch1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM3_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
const uint8_t id = 0x42;

// MAGIC
		/* Registers */
		#define REG_GAIN	0x00	/* Gain lower 8 bits (rest in vref) */
		#define REG_BLUE	0x01	/* blue gain */
		#define REG_RED		0x02	/* red gain */
		#define REG_VREF	0x03	/* Pieces of GAIN, VSTART, VSTOP */
		#define REG_COM1	0x04	/* Control 1 */
		#define  COM1_CCIR656	  0x40  /* CCIR656 enable */
		#define REG_BAVE	0x05	/* U/B Average level */
		#define REG_GbAVE	0x06	/* Y/Gb Average level */
		#define REG_AECHH	0x07	/* AEC MS 5 bits */
		#define REG_RAVE	0x08	/* V/R Average level */
		#define REG_COM2	0x09	/* Control 2 */
		#define  COM2_SSLEEP	  0x10	/* Soft sleep mode */
		#define REG_PID		0x0a	/* Product ID MSB */
		#define REG_VER		0x0b	/* Product ID LSB */
		#define REG_COM3	0x0c	/* Control 3 */
		#define  COM3_SWAP	  0x40	  /* Byte swap */
		#define  COM3_SCALEEN	  0x08	  /* Enable scaling */
		#define  COM3_DCWEN	  0x04	  /* Enable downsamp/crop/window */
		#define REG_COM4	0x0d	/* Control 4 */
		#define REG_COM5	0x0e	/* All "reserved" */
		#define REG_COM6	0x0f	/* Control 6 */
		#define REG_AECH	0x10	/* More bits of AEC value */
		#define REG_CLKRC	0x11	/* Clocl control */
		#define   CLK_EXT	  0x40	  /* Use external clock directly */
		#define   CLK_SCALE	  0x3f	  /* Mask for internal clock scale */
		#define REG_COM7	0x12	/* Control 7 */
		#define   COM7_RESET	  0x80	  /* Register reset */
		#define   COM7_FMT_MASK	  0x38
		#define   COM7_FMT_VGA	  0x00
		#define	  COM7_FMT_CIF	  0x20	  /* CIF format */
		#define   COM7_FMT_QVGA	  0x10	  /* QVGA format */
		#define   COM7_FMT_QCIF	  0x08	  /* QCIF format */
		#define	  COM7_RGB	  0x04	  /* bits 0 and 2 - RGB format */
		#define	  COM7_YUV	  0x00	  /* YUV */
		#define	  COM7_BAYER	  0x01	  /* Bayer format */
		#define	  COM7_PBAYER	  0x05	  /* "Processed bayer" */
		#define REG_COM8	0x13	/* Control 8 */
		#define   COM8_FASTAEC	  0x80	  /* Enable fast AGC/AEC */
		#define   COM8_AECSTEP	  0x40	  /* Unlimited AEC step size */
		#define   COM8_BFILT	  0x20	  /* Band filter enable */
		#define   COM8_AGC	  0x04	  /* Auto gain enable */
		#define   COM8_AWB	  0x02	  /* White balance enable */
		#define   COM8_AEC	  0x01	  /* Auto exposure enable */
		#define REG_COM9	0x14	/* Control 9  - gain ceiling */
		#define REG_COM10	0x15	/* Control 10 */
		#define   COM10_HSYNC	  0x40	  /* HSYNC instead of HREF */
		#define   COM10_PCLK_HB	  0x20	  /* Suppress PCLK on horiz blank */
		#define   COM10_HREF_REV  0x08	  /* Reverse HREF */
		#define   COM10_VS_LEAD	  0x04	  /* VSYNC on clock leading edge */
		#define   COM10_VS_NEG	  0x02	  /* VSYNC negative */
		#define   COM10_HS_NEG	  0x01	  /* HSYNC negative */
		#define REG_HSTART	0x17	/* Horiz start high bits */
		#define REG_HSTOP	0x18	/* Horiz stop high bits */
		#define REG_VSTART	0x19	/* Vert start high bits */
		#define REG_VSTOP	0x1a	/* Vert stop high bits */
		#define REG_PSHFT	0x1b	/* Pixel delay after HREF */
		#define REG_MIDH	0x1c	/* Manuf. ID high */
		#define REG_MIDL	0x1d	/* Manuf. ID low */
		#define REG_MVFP	0x1e	/* Mirror / vflip */
		#define   MVFP_MIRROR	  0x20	  /* Mirror image */
		#define   MVFP_FLIP	  0x10	  /* Vertical flip */

		#define REG_AEW		0x24	/* AGC upper limit */
		#define REG_AEB		0x25	/* AGC lower limit */
		#define REG_VPT		0x26	/* AGC/AEC fast mode op region */
		#define REG_HSYST	0x30	/* HSYNC rising edge delay */
		#define REG_HSYEN	0x31	/* HSYNC falling edge delay */
		#define REG_HREF	0x32	/* HREF pieces */
		#define REG_TSLB	0x3a	/* lots of stuff */
		#define   TSLB_YLAST	  0x04	  /* UYVY or VYUY - see com13 */
		#define REG_COM11	0x3b	/* Control 11 */
		#define   COM11_NIGHT	  0x80	  /* NIght mode enable */
		#define   COM11_NMFR	  0x60	  /* Two bit NM frame rate */
		#define   COM11_HZAUTO	  0x10	  /* Auto detect 50/60 Hz */
		#define	  COM11_50HZ	  0x08	  /* Manual 50Hz select */
		#define   COM11_EXP	  0x02
		#define REG_COM12	0x3c	/* Control 12 */
		#define   COM12_HREF	  0x80	  /* HREF always */
		#define REG_COM13	0x3d	/* Control 13 */
		#define   COM13_GAMMA	  0x80	  /* Gamma enable */
		#define	  COM13_UVSAT	  0x40	  /* UV saturation auto adjustment */
		#define   COM13_UVSWAP	  0x01	  /* V before U - w/TSLB */
		#define REG_COM14	0x3e	/* Control 14 */
		#define   COM14_DCWEN	  0x10	  /* DCW/PCLK-scale enable */
		#define REG_EDGE	0x3f	/* Edge enhancement factor */
		#define REG_COM15	0x40	/* Control 15 */
		#define   COM15_R10F0	  0x00	  /* Data range 10 to F0 */
		#define	  COM15_R01FE	  0x80	  /*            01 to FE */
		#define   COM15_R00FF	  0xc0	  /*            00 to FF */
		#define   COM15_RGB565	  0x10	  /* RGB565 output */
		#define   COM15_RGB555	  0x30	  /* RGB555 output */
		#define REG_COM16	0x41	/* Control 16 */
		#define   COM16_AWBGAIN   0x08	  /* AWB gain enable */
		#define REG_COM17	0x42	/* Control 17 */
		#define   COM17_AECWIN	  0xc0	  /* AEC window - must match COM4 */
		#define   COM17_CBAR	  0x08	  /* DSP Color bar */

		/*
		 * This matrix defines how the colors are generated, must be
		 * tweaked to adjust hue and saturation.
		 *
		 * Order: v-red, v-green, v-blue, u-red, u-green, u-blue
		 *
		 * They are nine-bit signed quantities, with the sign bit
		 * stored in 0x58.  Sign for v-red is bit 0, and up from there.
		 */
		#define	REG_CMATRIX_BASE 0x4f
		#define   CMATRIX_LEN 6
		#define REG_CMATRIX_SIGN 0x58


		#define REG_BRIGHT	0x55	/* Brightness */
		#define REG_CONTRAS	0x56	/* Contrast control */

		#define REG_GFIX	0x69	/* Fix gain control */

		#define REG_DBLV	0x6b	/* PLL control an debugging */
		#define   DBLV_BYPASS	  0x0a	  /* Bypass PLL */
		#define   DBLV_X4	  0x4a	  /* clock x4 */
		#define   DBLV_X6	  0x8a	  /* clock x6 */
		#define   DBLV_X8	  0xca	  /* clock x8 */

		#define REG_SCALING_XSC	0x70	/* Test pattern and horizontal scale factor */
		#define   TEST_PATTTERN_0 0x80
		#define REG_SCALING_YSC	0x71	/* Test pattern and vertical scale factor */
		#define   TEST_PATTTERN_1 0x80

		#define REG_REG76	0x76	/* OV's name */
		#define   R76_BLKPCOR	  0x80	  /* Black pixel correction enable */
		#define   R76_WHTPCOR	  0x40	  /* White pixel correction enable */

		#define REG_RGB444	0x8c	/* RGB 444 control */
		#define   R444_ENABLE	  0x02	  /* Turn on RGB444, overrides 5x5 */
		#define   R444_RGBX	  0x01	  /* Empty nibble at end */

		#define REG_HAECC1	0x9f	/* Hist AEC/AGC control 1 */
		#define REG_HAECC2	0xa0	/* Hist AEC/AGC control 2 */

		#define REG_BD50MAX	0xa5	/* 50hz banding step limit */
		#define REG_HAECC3	0xa6	/* Hist AEC/AGC control 3 */
		#define REG_HAECC4	0xa7	/* Hist AEC/AGC control 4 */
		#define REG_HAECC5	0xa8	/* Hist AEC/AGC control 5 */
		#define REG_HAECC6	0xa9	/* Hist AEC/AGC control 6 */
		#define REG_HAECC7	0xaa	/* Hist AEC/AGC control 7 */
		#define REG_BD60MAX	0xab	/* 60hz banding step limit */


const uint8_t regCfg[][2] = {
		{0x12, 0x80}, // Register Reset, Must wait 1ms

#ifdef LINM
		/*
			 * Set the hardware window.  These values from OV don't entirely
			 * make sense - hstop is less than hstart.  But they work...
			 */

			{ REG_HSTART, 0x13 },	{ REG_HSTOP, 0x01 },
			{ REG_HREF, 0xb6 },	{ REG_VSTART, 0x02 },
			{ REG_VSTOP, 0x7a },	{ REG_VREF, 0x0a },


			{ REG_COM3, 0 },	{ REG_COM14, 0 },

			/* Mystery scaling numbers */

			{ REG_SCALING_XSC, 0x3a },
			{ REG_SCALING_YSC, 0x35 },
			{ 0x72, 0x11 },		{ 0x73, 0xf0 },
			{ 0xa2, 0x02 },		{ REG_COM10, 0x0 },


			/* Gamma curve values */
			{ 0x7a, 0x20 },		{ 0x7b, 0x10 },
			{ 0x7c, 0x1e },		{ 0x7d, 0x35 },
			{ 0x7e, 0x5a },		{ 0x7f, 0x69 },
			{ 0x80, 0x76 },		{ 0x81, 0x80 },
			{ 0x82, 0x88 },		{ 0x83, 0x8f },
			{ 0x84, 0x96 },		{ 0x85, 0xa3 },
			{ 0x86, 0xaf },		{ 0x87, 0xc4 },
			{ 0x88, 0xd7 },		{ 0x89, 0xe8 },

			/* AGC and AEC parameters.  Note we start by disabling those features,
			   then turn them only after tweaking the values. */
			{ REG_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_BFILT },
			{ REG_GAIN, 0 },	{ REG_AECH, 0 },
			{ REG_COM4, 0x40 }, /* magic reserved bit */
			{ REG_COM9, 0x18 }, /* 4x gain + magic rsvd bit */
			{ REG_BD50MAX, 0x05 },	{ REG_BD60MAX, 0x07 },
			{ REG_AEW, 0x95 },	{ REG_AEB, 0x33 },
			{ REG_VPT, 0xe3 },	{ REG_HAECC1, 0x78 },
			{ REG_HAECC2, 0x68 },	{ 0xa1, 0x03 }, /* magic */
			{ REG_HAECC3, 0xd8 },	{ REG_HAECC4, 0xd8 },
			{ REG_HAECC5, 0xf0 },	{ REG_HAECC6, 0x90 },
			{ REG_HAECC7, 0x94 },
			{ REG_COM8, COM8_FASTAEC|COM8_AECSTEP|COM8_BFILT|COM8_AGC|COM8_AEC },

			/* Almost all of these are magic "reserved" values.  */
			{ REG_COM5, 0x61 },	{ REG_COM6, 0x4b },
			{ 0x16, 0x02 },		{ REG_MVFP, 0x07 },
			{ 0x21, 0x02 },		{ 0x22, 0x91 },
			{ 0x29, 0x07 },		{ 0x33, 0x0b },
			{ 0x35, 0x0b },		{ 0x37, 0x1d },
			{ 0x38, 0x71 },		{ 0x39, 0x2a },
			{ REG_COM12, 0x78 },	{ 0x4d, 0x40 },
			{ 0x4e, 0x20 },		{ REG_GFIX, 0 },
			{ 0x6b, 0x4a },		{ 0x74, 0x10 },
			{ 0x8d, 0x4f },		{ 0x8e, 0 },
			{ 0x8f, 0 },		{ 0x90, 0 },
			{ 0x91, 0 },		{ 0x96, 0 },
			{ 0x9a, 0 },		{ 0xb0, 0x84 },
			{ 0xb1, 0x0c },		{ 0xb2, 0x0e },
			{ 0xb3, 0x82 },		{ 0xb8, 0x0a },

			/* More reserved magic, some of which tweaks white balance */
			{ 0x43, 0x0a },		{ 0x44, 0xf0 },
			{ 0x45, 0x34 },		{ 0x46, 0x58 },
			{ 0x47, 0x28 },		{ 0x48, 0x3a },
			{ 0x59, 0x88 },		{ 0x5a, 0x88 },
			{ 0x5b, 0x44 },		{ 0x5c, 0x67 },
			{ 0x5d, 0x49 },		{ 0x5e, 0x0e },
			{ 0x6c, 0x0a },		{ 0x6d, 0x55 },
			{ 0x6e, 0x11 },		{ 0x6f, 0x9f }, /* "9e for advance AWB" */
			{ 0x6a, 0x40 },		{ REG_BLUE, 0x40 },
			{ REG_RED, 0x60 },
			{ REG_COM8, COM8_FASTAEC|COM8_AECSTEP|COM8_BFILT|COM8_AGC|COM8_AEC|COM8_AWB },

			/* Matrix coefficients */
			{ 0x4f, 0x80 },		{ 0x50, 0x80 },
			{ 0x51, 0 },		{ 0x52, 0x22 },
			{ 0x53, 0x5e },		{ 0x54, 0x80 },
			{ 0x58, 0x9e },


			{ REG_COM16, COM16_AWBGAIN },	{ REG_EDGE, 0 },
			{ 0x75, 0x05 },		{ 0x76, 0xe1 },
			{ 0x4c, 0 },		{ 0x77, 0x01 },

			//Something causing drift
			/*{ REG_COM13, 0xc3 },*/ {REG_COM13, 0xc9},	{ 0x4b, 0x09 },
			{ 0xc9, 0x60 },		{ REG_COM16, 0x38 },
			{ 0x56, 0x40 },

			{ 0x34, 0x11 },		{ REG_COM11, COM11_EXP|COM11_HZAUTO },
			{ 0xa4, 0x88 },		{ 0x96, 0 },
			{ 0x97, 0x30 },		{ 0x98, 0x20 },
			{ 0x99, 0x30 },		{ 0x9a, 0x84 },
			{ 0x9b, 0x29 },		{ 0x9c, 0x03 },
			{ 0x9d, 0x4c },		{ 0x9e, 0x3f },
			{ 0x78, 0x04 },

			/* Extra-weird stuff.  Some sort of multiplexor register */
			//SAFE
			{ 0x79, 0x01 },		{ 0xc8, 0xf0 },
			{ 0x79, 0x0f },		{ 0xc8, 0x00 },
			{ 0x79, 0x10 },		{ 0xc8, 0x7e },
			{ 0x79, 0x0a },		{ 0xc8, 0x80 },
			{ 0x79, 0x0b },		{ 0xc8, 0x01 },
			{ 0x79, 0x0c },		{ 0xc8, 0x0f },
			{ 0x79, 0x0d },		{ 0xc8, 0x20 },
			{ 0x79, 0x09 },		{ 0xc8, 0x80 },
			{ 0x79, 0x02 },		{ 0xc8, 0xc0 },
			{ 0x79, 0x03 },		{ 0xc8, 0x40 },
			{ 0x79, 0x05 },		{ 0xc8, 0x30 },
			{ 0x79, 0x26 },
#endif

/*
			 * Set the hardware window.  These values from OV don't entirely
			 * make sense - hstop is less than hstart.  But they work...
			 */

			{ REG_HSTART, 0x13 },	{ REG_HSTOP, 0x01 },
			{ REG_HREF, 0xb6 },	{ REG_VSTART, 0x02 },
			{ REG_VSTOP, 0x7a },	{ REG_VREF, 0x0a },


			{ REG_COM3, 0 },	{ REG_COM14, 0 },

			/* Mystery scaling numbers */

			{ REG_SCALING_XSC, 0x3a },
			{ REG_SCALING_YSC, 0x35 },
			{ 0x72, 0x11 },		{ 0x73, 0xf0 },
			{ 0xa2, 0x02 },		{ REG_COM10, 0x0 },

			/* Gamma curve values */
						{ 0x7a, 0x20 },		{ 0x7b, 0x10 },
						{ 0x7c, 0x1e },		{ 0x7d, 0x35 },
						{ 0x7e, 0x5a },		{ 0x7f, 0x69 },
						{ 0x80, 0x76 },		{ 0x81, 0x80 },
						{ 0x82, 0x88 },		{ 0x83, 0x8f },
						{ 0x84, 0x96 },		{ 0x85, 0xa3 },
						{ 0x86, 0xaf },		{ 0x87, 0xc4 },
						{ 0x88, 0xd7 },		{ 0x89, 0xe8 },

			/* AGC and AEC parameters.  Note we start by disabling those features,
						   then turn them only after tweaking the values. */
						{ REG_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_BFILT },
						{ REG_GAIN, 0 },	{ REG_AECH, 0 },
						{ REG_COM4, 0x40 }, /* magic reserved bit */
						{ REG_COM9, 0x18 }, /* 4x gain + magic rsvd bit */
						{ REG_BD50MAX, 0x05 },	{ REG_BD60MAX, 0x07 },
						{ REG_AEW, 0x95 },	{ REG_AEB, 0x33 },
						{ REG_VPT, 0xe3 },	{ REG_HAECC1, 0x78 },
						{ REG_HAECC2, 0x68 },	{ 0xa1, 0x03 }, /* magic */
						{ REG_HAECC3, 0xd8 },	{ REG_HAECC4, 0xd8 },
						{ REG_HAECC5, 0xf0 },	{ REG_HAECC6, 0x90 },
						{ REG_HAECC7, 0x94 },
						{ REG_COM8, COM8_FASTAEC|COM8_AECSTEP|COM8_BFILT|COM8_AGC|COM8_AEC },

			/* Almost all of these are magic "reserved" values.  */
						{ REG_COM5, 0x61 },	{ REG_COM6, 0x4b },
						{ 0x16, 0x02 },		{ REG_MVFP, 0x07 },
						{ 0x21, 0x02 },		{ 0x22, 0x91 },
						{ 0x29, 0x07 },		{ 0x33, 0x0b },
						{ 0x35, 0x0b },		{ 0x37, 0x1d },
						{ 0x38, 0x71 },		{ 0x39, 0x2a },
						{ REG_COM12, 0x78 },	{ 0x4d, 0x40 },
						{ 0x4e, 0x20 },		{ REG_GFIX, 0 },
						{ 0x6b, 0x4a },		{ 0x74, 0x10 },
						{ 0x8d, 0x4f },		{ 0x8e, 0 },
						{ 0x8f, 0 },		{ 0x90, 0 },
						{ 0x91, 0 },		{ 0x96, 0 },
						{ 0x9a, 0 },		{ 0xb0, 0x84 },
						{ 0xb1, 0x0c },		{ 0xb2, 0x0e },
						{ 0xb3, 0x82 },		{ 0xb8, 0x0a },

						/* More reserved magic, some of which tweaks white balance */
						{ 0x43, 0x0a },		{ 0x44, 0xf0 },
						{ 0x45, 0x34 },		{ 0x46, 0x58 },
						{ 0x47, 0x28 },		{ 0x48, 0x3a },
						{ 0x59, 0x88 },		{ 0x5a, 0x88 },
						{ 0x5b, 0x44 },		{ 0x5c, 0x67 },
						{ 0x5d, 0x49 },		{ 0x5e, 0x0e },
						{ 0x6c, 0x0a },		{ 0x6d, 0x55 },
						{ 0x6e, 0x11 },		{ 0x6f, 0x9f }, /* "9e for advance AWB" */
						{ 0x6a, 0x40 },		{ REG_BLUE, 0x40 },
						{ REG_RED, 0x60 },
						{ REG_COM8, COM8_FASTAEC|COM8_AECSTEP|COM8_BFILT|COM8_AGC|COM8_AEC|COM8_AWB },

						/* Matrix coefficients */
						{ 0x4f, 0x80 },		{ 0x50, 0x80 },
						{ 0x51, 0 },		{ 0x52, 0x22 },
						{ 0x53, 0x5e },		{ 0x54, 0x80 },
						{ 0x58, 0x9e },


						{ REG_COM16, COM16_AWBGAIN },	{ REG_EDGE, 0 },
						{ 0x75, 0x05 },		{ 0x76, 0xe1 },
						{ 0x4c, 0 },		{ 0x77, 0x01 },



#ifdef YUV
			// YUV linux
				{ REG_COM7, 0x0 },  /* Selects YUV mode */
					{ REG_RGB444, 0 },	/* No RGB444 please */
					{ REG_COM1, 0 },	/* CCIR601 */
					{ REG_COM15, COM15_R00FF },
					{ REG_COM9, 0x48 }, /* 32x gain ceiling; 0x8 is reserved bit */
					{ 0x4f, 0x80 },		/* "matrix coefficient 1" */
					{ 0x50, 0x80 },		/* "matrix coefficient 2" */
					{ 0x51, 0    },		/* vb */
					{ 0x52, 0x22 },		/* "matrix coefficient 4" */
					{ 0x53, 0x5e },		/* "matrix coefficient 5" */
					{ 0x54, 0x80 },		/* "matrix coefficient 6" */
					{ REG_COM13, COM13_GAMMA|COM13_UVSAT },

#endif


		//Our settings
		{0x11, 0x9F}, // Clock Prescaler
		{0x6B, 0x7A}, // PLL x4
	//    /*{0x2A, 0xF0},*/ {0x2B, 0xFF}, // Dummy pixel insert
	    {0x15, 0x20}, // Don't toggle HSYNC during horizontal blank
	//	{0x92, 0xFF}, {0x93, 0x07}, //Dummy Row
		  {0x0C, 0x08}, {0x12, 0x08}, //QCIF
		// {0x40, 0x80},// {0x04, 0x40}, // CIR656
	 // /* {0x70, 0x4A | 0x80},*/ {0x71, 0x35 | 0x80}, // Test Pattern
	//	{0x1B, 0xFF}, //Pixel delay to HREF

/*
		//{0x11, 0x01},
		{0x12, 0x00},
		{0x0C, 0x0C},
		{0x32, 0x11},
		{0x70, 0x3A},
		{0x71, 0x35 | 0x80 },
		{0x72, 0x11},
		{0x73, 0xF1},
		{0xA2, 0x52},*/
		// {0x13, 0x08}, //Turn off auto
		// {0x14, 0x6A}, //Gain ceil 4x
		//{0xAA, 0x04}, //Avg AEC
		// {0x41, 0x38}, //Edge enhance, denoise, AWB Gain
		//{0xB1, 0x04}, //Black level
	    //{0x55, 0x8F}, //Brightness
		//{0x26, 0x76}, //Small control region
		//{0x3D, 0xD9},
		// {0x13, 0xCF}, //TUrn on auto

};

void log(char *c)
{
	HAL_UART_Transmit(&huart2, c, strlen(c), 1000000);
}

uint8_t camReadReg(uint8_t memAddr)
{
	int t = HAL_GetTick();
	HAL_I2C_Master_Transmit(&hi2c1, id, &memAddr, 1, 1000);
	CLEAR_BIT(hi2c1.Instance->CR1, I2C_CR1_ACK);
		SET_BIT(hi2c1.Instance->CR1, I2C_CR1_START);
		while (READ_BIT(hi2c1.Instance->SR1, I2C_SR1_SB) == RESET && HAL_GetTick() - t < 1000);
		hi2c1.Instance->DR = id | 1;
		while (READ_BIT(hi2c1.Instance->SR1, I2C_SR1_ADDR) == RESET && HAL_GetTick() - t < 1000);
		__HAL_I2C_CLEAR_ADDRFLAG(&hi2c1);
		while (READ_BIT(hi2c1.Instance->SR1, I2C_SR1_RXNE) == RESET && HAL_GetTick() - t < 1000);
		uint8_t data = hi2c1.Instance->DR;
		SET_BIT(hi2c1.Instance->CR1, I2C_CR1_STOP);
		if (HAL_GetTick() - t >= 1000)
			return 0;
		return data;
}

int camInit()
{

	for (int i = 0; i < sizeof(regCfg)/sizeof(regCfg[0]); i++) {
		int err;
		while ((err = HAL_I2C_Master_Transmit(&hi2c1, id, regCfg[i], 2, 1000)) != HAL_OK) {

			char buff[200];
			int l = sprintf(buff, "ERR: A Register set fail: %d ERR=%d Retrying\r\n", i, err);
			HAL_UART_Transmit(&huart2, buff, l, 1000);
			HAL_I2C_DeInit(&hi2c1);
				  	  HAL_Delay(1000);
				  	  HAL_I2C_Init(&hi2c1);
		}
		HAL_Delay(10);

		/*
		int v = camReadReg(regCfg[i][0]);
		if (i != 0 &&  v != regCfg[i][1]) {
			log("Bit Error\r\n");
			return 0;
		}
		*/


	}

	/*
	char v = 0xFF;
		if (HAL_I2C_Mem_Read(&hi2c1, id, 0x01, I2C_MEMADD_SIZE_8BIT, &v, 1, 1000) != HAL_OK) {
					return 0;
		}
		if (v != 0x80)
			return 0;
			*/
	return 1;
}

// (174x2)x144
#define IMG_B_WIDTH 348
#define IMG_B_HEIGHT 144
#define IMG_BUFFER_BYTES (IMG_B_WIDTH*IMG_B_HEIGHT)
uint32_t imgBuff[IMG_BUFFER_BYTES/4];
int frameStart;

void camBuffErr(DMA_HandleTypeDef *dma)
{
	log("Error: Camera DMA failed");
}

int horzAlignTim;
int horzLine[200];
int horzAp = -1;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim2) {
		if (horzAp != -1 ) {
			if (horzAp < 200)
				horzLine[horzAp] =  htim->Instance->CCR2;
			horzAp++;
		}
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  //PWM 10.5 MHz on pin D5
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  //Input Capture on pin D6
  __HAL_TIM_ENABLE_DMA(&htim1, TIM_DMA_CC1);
  HAL_TIM_IC_Start(&htim1, TIM_CHANNEL_1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_Delay(5000);
  while (!camInit()) {
	  	  log("Error during camera register initialization\r\n");
	  	  HAL_I2C_DeInit(&hi2c1);
	  	  HAL_Delay(1000);
	  	  HAL_I2C_Init(&hi2c1);
  }

  if (HAL_DMA_RegisterCallback(&hdma_tim1_ch1, HAL_DMA_XFER_ERROR_CB_ID, &camBuffErr) != HAL_OK)
	  log("Error during dma cam cb err regis");
  if (HAL_DMA_RegisterCallback(&hdma_tim1_ch1, HAL_DMA_XFER_ABORT_CB_ID, &camBuffErr) != HAL_OK)
  	  log("Error during dma cam cb abt regis");

  HAL_DMA_Start(&hdma_tim1_ch1, &(GPIOC->IDR), imgBuff, IMG_BUFFER_BYTES);
  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */



	  /*
	int prv = htim2.Instance->CCR3;
	while (((htim2.Instance->SR >> 3) & 1) == 0);
	int nxt = htim2.Instance->CCR3;
	char buff[500];
	int l = sprintf(buff, "Cnt = %d\r\n", nxt-prv);
	HAL_UART_Transmit(&huart2, buff, l, 1000000);
	*/
	  /*

	  for (int i = 0; i < IMG_BUFFER_BYTES; i++)
	  		imgBuff[i] = 0xBA;
	  camState = REQUEST;
	while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10) == GPIO_PIN_RESET);
	HAL_DMA_Start_IT(&hdma_tim1_ch1, &(GPIOC->IDR), imgBuff, IMG_BUFFER_BYTES);
	lineCnt = 0;
	//int prv = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5);
	while (HAL_DMA_GetState(&hdma_tim1_ch1) == HAL_DMA_STATE_BUSY || camState != IDLE) {
		while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_RESET);
		while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_SET);
		lineCnt++;
		//prv = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5);
	}
	*/
/*
	t[0] = 0;
	for (int i = 1; i < sizeof(t); i++) {

		int v;
		do {
			v = (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10) << 1) | HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5);
		} while (v == t[i-1]);

		t[i] = v;
	}

	continue;
	*/


	//while (hdma_tim1_ch1.Instance->CR & 1);

	  //HAL_DMA_Start_IT(&hdma_tim1_ch1, &(GPIOC->IDR), imgBuff, IMG_BUFFER_BYTES);
	 // while (camState != IDLE);
	  for (int i = 0; i < IMG_BUFFER_BYTES/4; i++)
	  	 		imgBuff[i] = 0x00;


	  int  horzVl;

	 __HAL_TIM_ENABLE(&htim1);
	 HAL_Delay(1000);

	 while (!(GPIOA->IDR & (1 << 10)))asm("NOP"); // Wait for VSNYC high
	 int frameStart = IMG_BUFFER_BYTES - hdma_tim1_ch1.Instance->NDTR;
	 //horzAlignTim = htim2.Instance->CNT;
	 horzAp = 0;
	 htim2.Instance->CNT = IMG_BUFFER_BYTES - hdma_tim1_ch1.Instance->NDTR;
	 while (GPIOA->IDR & (1 << 10))asm("NOP"); // Wait for VSNYC low
	 /*int cP = hdma_tim1_ch1.Instance->NDTR;
	 do {
		 cP = hdma_tim1_ch1.Instance->NDTR;
	 } while (cP > frameStart || frameStart - cP > 100);
	  */
	 while (!(GPIOA->IDR & (1 << 10)))asm("NOP"); // Wait for VSNYC high
	 __HAL_TIM_DISABLE(&htim1);
	 horzVl = horzAp;
	 horzAp = -1;

	 if (horzVl != IMG_B_HEIGHT) {
		 log("Retrying\r\n");
		 char buff[200];
		 int l =sprintf(buff, "HCNT = %d\r\n", horzVl);
		 HAL_UART_Transmit(&huart2, buff, l, 1000);
	 }
		 // Wait for loop
	 /*for (int i = 0; i < 144; i++) {
		 while (!(GPIOB->IDR & ( 1<< 5))); //Wait for HSYNC to go low
		 lineStop[i] = IMG_BUFFER_BYTES - hdma_tim1_ch1.Instance->NDTR;
		 while (GPIOB->IDR & ( 1<< 5)); //Wait for HSYNC to go high
	 }*/

/*
	 while (!(GPIOA->IDR & (1 << 10)))asm("NOP"); // Wait for VSNYC high
	 frameStart = 0;
	 while (GPIOA->IDR & (1 << 10))asm("NOP"); // Wait for VSNYC low
	 int cp = 0;
	 while (cp != IMG_BUFFER_BYTES) {
		 if (cp % 174*2 == 0) {
		 			 while ((GPIOA->IDR &2)) asm("NOP");
		 		 }
		 while (!(GPIOA->IDR & (1 << 8))) asm("NOP");
		 ((uint8_t*) imgBuff)[cp] = GPIOC->IDR & 0xFF;
		 cp++;
		 while ((GPIOA->IDR & (1 << 8))) asm("NOP");

	 }
*/


	 char *trig = "TRIGTRIGTRIG\r\n";
	 HAL_UART_Transmit(&huart2, trig, strlen(trig), 1000000);
	 /* for (int i = 0; i < 144; i++) {
		 HAL_UART_Transmit(&huart2, ((uint8_t*) imgBuff) + lineStop[i], 349 , 1000000);
	 }
	 */

	 /*
	 HAL_UART_Transmit(&huart2, ((uint8_t*) imgBuff) + frameStart, IMG_BUFFER_BYTES - frameStart, 1000000);
	 HAL_UART_Transmit(&huart2, ((uint8_t*) imgBuff), frameStart, 1000000);
	*/
	 uint8_t *bp = ((uint8_t*) imgBuff);
	 for (int i = 0; i < IMG_B_HEIGHT; i++) {
		 int mk = (i < horzVl ? horzLine[i] % IMG_BUFFER_BYTES : ((frameStart + IMG_B_WIDTH * i) % IMG_BUFFER_BYTES));
		 int avail = IMG_BUFFER_BYTES - mk;
		 if (avail > IMG_B_WIDTH)
			 avail = IMG_B_WIDTH;
		 HAL_UART_Transmit(&huart2, bp + mk, avail, 1000);
		 if (avail < IMG_B_WIDTH)
			 HAL_UART_Transmit(&huart2, bp, IMG_B_WIDTH - avail, 1000);
	 }

	 log("\r\n");


/*

	 while (hdma_tim1_ch1.Instance->CR & 1){
		 if (((GPIOB->IDR >> 5) &1) == 0) {
			 lineStop[lineCnt++] = IMG_BUFFER_BYTES - hdma_tim1_ch1.Instance->NDTR;
			while (((GPIOB->IDR >> 5) &1) == 0);
		 }
		 if (((GPIOA->IDR >> 10) & 1) == 1) {
			 while (((GPIOA->IDR >> 10) & 1) == 1);
			 frameSk[frameSkipped++] = frameStart = IMG_BUFFER_BYTES - hdma_tim1_ch1.Instance->NDTR;
		 }
	 }
	 */
	 // camState = REQUEST;


  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 1000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
  sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sSlaveConfig.TriggerFilter = 0;
  if (HAL_TIM_SlaveConfigSynchro(&htim1, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
  sSlaveConfig.InputTrigger = TIM_TS_ITR0;
  if (HAL_TIM_SlaveConfigSynchro(&htim2, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 7;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 5;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PC0 PC1 PC2 PC3
                           PC4 PC5 PC6 PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
