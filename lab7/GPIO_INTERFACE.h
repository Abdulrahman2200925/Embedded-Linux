#ifndef GPIO_INTERFACE_H
#define GPIO_INTERFACE_H    

#ifdef __cplusplus
extern "C" {
#endif

#include  "STD_TYPES.h"


typedef enum {
    GPIO_OK                       = 0x00U,   // Operation successful
    GPIO_ERROR_INVALID_PIN        = 0x01U,   // Invalid pin number or port
    GPIO_ERROR_INVALID_MODE       = 0x02U,   // Invalid mode (input/output/AF/analog)
    GPIO_ERROR_INVALID_SPEED      = 0x03U,   // Invalid speed configuration
    GPIO_ERROR_INVALID_PUPD       = 0x04U,   // Invalid pull-up/pull-down configuration
    GPIO_ERROR_INVALID_AF         = 0x05U,   // Invalid alternate function selection
    GPIO_ERROR_NULL_PTR           = 0x06U,   // Null pointer passed to function
    GPIO_ERROR_PIN_LOCKED         = 0x07U,   // Pin is locked, cannot modify configuration
    GPIO_ERROR_PORT_NOT_ENABLED   = 0x08U,   // Corresponding port clock not enabled
    GPIO_ERROR_WRITE_PROTECTED    = 0x09U,   // Attempt to write to a protected pin (e.g. JTAG)
    GPIO_ERROR_TIMEOUT            = 0x0AU,   // Operation timeout (e.g. during lock/unlock)
    GPIO_ERROR_UNKNOWN            = 0xFFU    // Unknown or unexpected error
} GPIO_Error_t;


typedef enum{

GPIO_PIN0,
GPIO_PIN1,
GPIO_PIN2,          
GPIO_PIN3,
GPIO_PIN4,
GPIO_PIN5,
GPIO_PIN6,      
GPIO_PIN7,
GPIO_PIN8,
GPIO_PIN9,
GPIO_PIN10,
GPIO_PIN11,
GPIO_PIN12, 
GPIO_PIN13,
GPIO_PIN14,
GPIO_PIN15, 
}GPIO_PIN_t;

typedef enum{
GPIO_PIN_RESET = 0,
GPIO_PIN_SET    
} GPIO_PinState_t;

typedef enum{
GPIO_PORTA,
GPIO_PORTB,
GPIO_PORTC,         
GPIO_PORTD,
GPIO_PORTE,
GPIO_PORTH,
}GPIO_PORT_t;

typedef enum
{
    GPIO_MODE_INPUT ,
    GPIO_MODE_OUTPUT ,
    GPIO_MODE_ALTERNATE ,
    GPIO_MODE_ANALOG    
} GPIO_Mode_t;

typedef enum
{
    GPIO_OUTPUT_PUSH_PULL ,
    GPIO_OUTPUT_OPEN_DRAIN    
} GPIO_OutputType_t;

typedef enum
{
    GPIO_NO_PULL ,
    GPIO_PULL_UP ,
    GPIO_PULL_DOWN     
} GPIO_PullType_t;

typedef enum
{
    GPIO_SPEED_LOW ,
    GPIO_SPEED_MEDIUM ,
    GPIO_SPEED_HIGH ,
    GPIO_SPEED_VERY_HIGH     
} GPIO_Speed_t;

typedef enum
{
    AF_SYSTEM ,
    AF_TIM_1_2 ,
    AF_TIM_3_5 ,
    AF_TIM_9_11 ,
    AF_I2C_1_3 ,
    AF_SPI_1_4 ,
    AF_SPI_3 ,
    AF_USART_1_2 ,
    AF_USART_6 ,
    AF_I2C_2_3 ,
    AF_OTG_FS ,
    AF_SDIO ,
    AF_EVENTOUT 
    
} GPIO_AlternateFunction_t;

typedef struct
{
    GPIO_PIN_t        Pin;        // Pin number: e.g. GPIO_PIN_0, GPIO_PIN_5
    GPIO_PORT_t      Port;       // Port: e.g. GPIO_PORTA, GPIO_PORTB
    GPIO_Mode_t       Mode;       // Input, Output, AF, Analog
    GPIO_OutputType_t Type;       // Push-pull or Open-drain
    GPIO_Speed_t      Speed;      // Low, Medium, High, Very High
    GPIO_PullType_t       Pull;       // No pull, Pull-up, Pull-down
    GPIO_AlternateFunction_t           Alternate;  // AF number (if AF mode)
} GPIO_CONFIG_t;






GPIO_Error_t GPIO_INIT(GPIO_CONFIG_t *GPIO_Config);
GPIO_Error_t GPIO_DeInit(GPIO_PORT_t Port, GPIO_PIN_t Pin);
GPIO_Error_t GPIO_ReadPin(GPIO_PORT_t Port, GPIO_PIN_t Pin, uint8_t *State);
GPIO_Error_t GPIO_WritePin(GPIO_PORT_t Port, GPIO_PIN_t Pin, GPIO_PinState_t State);
GPIO_Error_t GPIO_TogglePin(GPIO_PORT_t Port, GPIO_PIN_t Pin);
GPIO_Error_t GPIO_LockPin(GPIO_PORT_t Port, GPIO_PIN_t Pin);
GPIO_Error_t GPIO_UnlockPin(GPIO_PORT_t Port, GPIO_PIN_t Pin);
uint8_t GPIO_ReadPinFast(GPIO_PORT_t Port, GPIO_PIN_t Pin);


#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
namespace mcal {
namespace gpio {

// C++ wrapper API (namespaced versions)
GPIO_Error_t init(GPIO_CONFIG_t *cfg);
GPIO_Error_t deinit(GPIO_PORT_t port, GPIO_PIN_t pin);
GPIO_Error_t write(GPIO_PORT_t port, GPIO_PIN_t pin, GPIO_PinState_t state);
GPIO_Error_t read(GPIO_PORT_t port, GPIO_PIN_t pin, uint8_t *value);
GPIO_Error_t toggle(GPIO_PORT_t port, GPIO_PIN_t pin);
GPIO_Error_t lock(GPIO_PORT_t port, GPIO_PIN_t pin);
GPIO_Error_t unlock(GPIO_PORT_t port, GPIO_PIN_t pin);
uint8_t      readFast(GPIO_PORT_t port, GPIO_PIN_t pin);

} // namespace gpio
} // namespace mcal
#endif






#endif // GPIO_INTERFACE_H