#ifndef __INC_CLOCKLESS_CSKY_W80X_H
#define __INC_CLOCKLESS_CSKY_W80X_H


#define SYS_I2S_CLK        (160000000)
/*
 * This clockless implementation uses I2s bus with DMA handler
 */

FASTLED_NAMESPACE_BEGIN
#define FASTLED_HAS_CLOCKLESS 1

#if FASTLED_W80X_CLOCKLESS_I2S
/*static CMinWait<0> *dma_chan_waits[NUM_DMA_CHANNELS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static inline void __isr clockless_dma_complete_handler() {
    for (unsigned int i = 0; i < NUM_DMA_CHANNELS; i++) {
        // if dma triggered for this channel and it's been used (has a CMinWait)
        if ((dma_hw->ints0 & (1 << i)) && dma_chan_waits[i]) {
            dma_hw->ints0 = (1 << i); // clear/ack IRQ
            dma_chan_waits[i]->mark(); // mark the wait
            return;
        }
    }
}
static bool clockless_isr_installed = false;*/
#endif

template <uint8_t DATA_PIN, int T1, int T2, int T3, EOrder RGB_ORDER = RGB, int XTRA0 = 0, bool FLIP = false, int WAIT_TIME = 50>
class ClocklessController : public CPixelLEDController<RGB_ORDER> {
#if FASTLED_W80X_CLOCKLESS_I2S
    I2S_HandleTypeDef hi2s;
    uint32_t *dma_buf = nullptr;
    size_t dma_buf_size = 0;
 // writes bits to an in-memory buffer (to DMA from)
 // w80x has enough memory to not really care about using a buffer for DMA
    template<int BITS> __attribute__ ((always_inline)) inline static int writeBitsToBuf(uint32_t *out_buf, unsigned int bitpos, uint8_t b)  {
        
        uint32_t pattern = (0b10010010 << 16)|(0b01001001<< 8)|0b00100100;
        // position of word that takes highest bits (first word used)
        int wordpos = bitpos >> 3; // bitpos / 8;
        
         if (b != 0) {
           uint8_t i =1; 
           uint8_t j =0; 
           while (j < 8) {
             if (b & i) pattern |= (1 << (j*3 +1));
             j++;
             i = i << 1;
             }
          }   
        out_buf[wordpos] = pattern;
        
        return BITS;
    }
#endif

public:
    virtual void init() {

        
        // start by configuring pin as output for blocking fallback
        FastPin<DATA_PIN>::setOutput();
#if FASTLED_W80X_CLOCKLESS_I2S
    hi2s.Instance = I2S;
    hi2s.Init.Mode = I2S_MODE_MASTER;
    hi2s.Init.Standard = I2S_STANDARD_PHILIPS;
    hi2s.Init.DataFormat = I2S_DATAFORMAT_24B;
    hi2s.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
    hi2s.Init.AudioFreq = I2S_AUDIOFREQ_44K;
    hi2s.Init.Channel = I2S_CHANNEL_STEREO;
    hi2s.Init.ChannelSel = I2S_CHANNELSEL_LEFT;
    if (HAL_I2S_Init(&hi2s) != HAL_OK)
    {
       // Error_Handler();
    }
    uint32_t ws_div = round((SYS_I2S_CLK / (50000ul * 48)));
    MODIFY_REG(RCC->I2S_CLK, RCC_I2S_CLK_BCLKDIV, (ws_div << RCC_I2S_CLK_BCLKDIV_Pos));
     __HAL_RCC_I2S_CLK_ENABLE();
        __HAL_RCC_GPIO_CLK_ENABLE();
        
        //__HAL_AFIO_REMAP_I2S_MCK(GPIOA, GPIO_PIN_7);
        //__HAL_AFIO_REMAP_I2S_WS(GPIOB, GPIO_PIN_9);
       // __HAL_AFIO_REMAP_I2S_CK(GPIOB, GPIO_PIN_8);
        __HAL_AFIO_REMAP_I2S_MOSI(FastPin<DATA_PIN>::port(), FastPin<DATA_PIN>::mask());
        //__HAL_AFIO_REMAP_I2S_MISO(GPIOB, GPIO_PIN_10);
#endif

    }

    virtual uint16_t getMaxRefreshRate() const { return 400; }

    virtual void showPixels(PixelController<RGB_ORDER> & pixels) {

#if FASTLED_W80X_CLOCKLESS_I2S
showRGBInternal(pixels);
#endif

    }
#if FASTLED_W80X_CLOCKLESS_I2S
 void showRGBInternal(PixelController<RGB_ORDER> pixels) {
        size_t req_buf_size = pixels.mLen * 3;
        
        // (re)allocate DMA buffer if not large enough to hold req_buf_size 32-bit words
        // pico has enough memory to not really care about using a buffer for DMA
        // just give up on failure
        if (dma_buf_size < req_buf_size) {
            if (dma_buf != nullptr)
                free(dma_buf);
            
            dma_buf = (uint32_t*) malloc(req_buf_size * 4);
            if (dma_buf == nullptr) {
                dma_buf_size = 0;
                return;
            }
            dma_buf_size = req_buf_size;
            
            // fill with zeroes to ensure XTRA0s are really zero without needing extra work
            memset(dma_buf, 0, dma_buf_size * 4);
        }
        
        unsigned int bitpos = 0;
        
        pixels.preStepFirstByteDithering();
        uint8_t b = pixels.loadAndScale0();
        
        while(pixels.has(1)) {
            pixels.stepDithering();

            // Write first byte, read next byte
            bitpos += writeBitsToBuf<8>((uint32_t*)(dma_buf), bitpos, b);
            b = pixels.loadAndScale1();

            // Write second byte, read 3rd byte
            bitpos += writeBitsToBuf<8>((uint32_t*)(dma_buf), bitpos, b);
            b = pixels.loadAndScale2();

            // Write third byte, read 1st byte of next pixel
            bitpos += writeBitsToBuf<8>((uint32_t*)(dma_buf), bitpos, b);
            b = pixels.advanceAndLoadAndScale0();
        };
        HAL_I2S_Transmit(&hi2s, (uint32_t*) dma_buf, req_buf_size, 1000);
        //do_dma_transfer(dma_channel, dma_buf, req_buf_size);
    }
#endif

  /*  
    void showRGBBlocking(PixelController<RGB_ORDER> pixels) {
        struct M0ClocklessData data;
        data.d[0] = pixels.d[0];
        data.d[1] = pixels.d[1];
        data.d[2] = pixels.d[2];
        data.s[0] = pixels.mScale[0];
        data.s[1] = pixels.mScale[1];
        data.s[2] = pixels.mScale[2];
        data.e[0] = pixels.e[0];
        data.e[1] = pixels.e[1];
        data.e[2] = pixels.e[2];
        data.adj = pixels.mAdvance;

        typedef FastPin<DATA_PIN> pin;
        volatile uint32_t *portBase = &sio_hw->gpio_out;
        const int portSetOff = (uint32_t)&sio_hw->gpio_set - (uint32_t)&sio_hw->gpio_out;
        const int portClrOff = (uint32_t)&sio_hw->gpio_clr - (uint32_t)&sio_hw->gpio_out;
        
        cli();
        showLedData<portSetOff, portClrOff, T1, T2, T3, RGB_ORDER, WAIT_TIME>(portBase, pin::mask(), pixels.mData, pixels.mLen, &data);
        sei();
    }
*/
};

FASTLED_NAMESPACE_END


#endif // __INC_CLOCKLESS_CSKY_W80X_H
