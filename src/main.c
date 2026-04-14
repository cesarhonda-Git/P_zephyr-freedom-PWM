#include <zephyr/kernel.h>             // Funções básicas do Zephyr (ex: k_msleep, k_thread, etc.)
#include <zephyr/device.h>             // API para obter e utilizar dispositivos do sistema
#include <zephyr/drivers/gpio.h>       // API para controle de pinos de entrada/saída (GPIO)
#include <pwm_z42.h>                // Biblioteca personalizada com funções de controle do TPM (Timer/PWM Module)

#define SLEEP_TIME_MS 1000 //intervalo de 1 segundo

const struct device *gpioa_dev = DEVICE_DT_GET(DT_NODELABEL(gpioa)); //devise tree
const struct device *gpiob_dev = DEVICE_DT_GET(DT_NODELABEL(gpiob));



// Define o valor do registrador MOD do TPM para configurar o período do PWM
#define TPM_MODULE 1000         // Define a frequência do PWM fpwm = (TPM_CLK / (TPM_MODULE * PS))
// Valores de duty cycle correspondentes a diferentes larguras de pulso
uint16_t duty_100 = 0;
uint16_t duty_75 = TPM_MODULE/4; 
uint16_t duty_50  = TPM_MODULE/2;       // 50% de duty cycle (meio brilho)
uint16_t duty_25 = TPM_MODULE/4 *3;
uint16_t duty_0 = TPM_MODULE;


int main(void)
{
    int ondi = 1, ones = 2;
    // Inicializa o módulo TPM2 com:
    // - base do TPMx
    // - fonte de clock PLL/FLL (TPM_CLK)
    // - valor do registrador MOD
    // - tipo de clock (TPM_CLK)
    // - prescaler de 1 a 128 (PS)
    // - modo de operação EDGE_PWM
    pwm_tpm_Init(TPM0, TPM_PLLFLL, TPM_MODULE, TPM_CLK, PS_128, EDGE_PWM);

    // Inicializa o canal 0 do TPM0 para gerar sinal PWM na porta GPIOB_18
    // - modo TPM_PWM_H (nível alto durante o pulso)
    pwm_tpm_Ch_Init(TPM0, ondi, TPM_PWM_H, GPIOA, 4); //enab direita
    pwm_tpm_Ch_Init(TPM0, ones, TPM_PWM_H, GPIOA, 5); //enab esquerda

    gpio_pin_configure(gpioa_dev, 12, GPIO_OUTPUT_ACTIVE);
    gpio_pin_configure(gpioa_dev, 13, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure(gpiob_dev, 0, GPIO_OUTPUT_ACTIVE);
    gpio_pin_configure(gpiob_dev, 1, GPIO_OUTPUT_INACTIVE);

    // Loop infinito
    for (;;)
    {
        // O programa poderia alterar o duty cycle dinamicamente aqui se desejado
        pwm_tpm_CnV(TPM0, ondi, duty_0);
        pwm_tpm_CnV(TPM0, ones, duty_0);
        k_msleep(SLEEP_TIME_MS*3);
        
        pwm_tpm_CnV(TPM0, ondi, duty_25);
        pwm_tpm_CnV(TPM0, ones, duty_25);
        k_msleep(SLEEP_TIME_MS*3);

        pwm_tpm_CnV(TPM0, ondi, duty_50);
        pwm_tpm_CnV(TPM0, ones, duty_50);
        k_msleep(SLEEP_TIME_MS*3);

        pwm_tpm_CnV(TPM0, ondi, duty_75);
        pwm_tpm_CnV(TPM0, ones, duty_75);
        k_msleep(SLEEP_TIME_MS*3);

        pwm_tpm_CnV(TPM0, ondi, duty_100);
        pwm_tpm_CnV(TPM0, ones, duty_100);
        k_msleep(SLEEP_TIME_MS*3);
    }

    return 0;
}