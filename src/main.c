#include <zephyr/kernel.h>             // Funções básicas do Zephyr (ex: k_msleep, k_thread, etc.)
#include <zephyr/device.h>             // API para obter e utilizar dispositivos do sistema
#include <zephyr/drivers/gpio.h>       // API para controle de pinos de entrada/saída (GPIO)
#include <pwm_z42.h>                // Biblioteca personalizada com funções de controle do TPM (Timer/PWM Module)

#define SLEEP_TIME_MS 1000 //intervalo de 1 segundo

// Define o LED usando Device Tree
#define LED0_NODE DT_ALIAS(led0)

// Define o valor do registrador MOD do TPM para configurar o período do PWM
#define TPM_MODULE 1000         // Define a frequência do PWM fpwm = (TPM_CLK / (TPM_MODULE * PS))
// Valores de duty cycle correspondentes a diferentes larguras de pulso
uint16_t duty_50  = TPM_MODULE/2;       // 50% de duty cycle (meio brilho)
uint16_t duty_75 = TPM_MODULE/4 *3;
uint16_t duty_100 = TPM_MODULE;


int main(void)
{
    int red = 0, green = 1;
    // Inicializa o módulo TPM2 com:
    // - base do TPMx
    // - fonte de clock PLL/FLL (TPM_CLK)
    // - valor do registrador MOD
    // - tipo de clock (TPM_CLK)
    // - prescaler de 1 a 128 (PS)
    // - modo de operação EDGE_PWM
    pwm_tpm_Init(TPM2, TPM_PLLFLL, TPM_MODULE, TPM_CLK, PS_128, EDGE_PWM);

    // Inicializa o canal 0 do TPM2 para gerar sinal PWM na porta GPIOB_18
    // - modo TPM_PWM_H (nível alto durante o pulso)
    pwm_tpm_Ch_Init(TPM2, red, TPM_PWM_H, GPIOB, 18);

    pwm_tpm_Ch_Init(TPM2, green, TPM_PWM_H, GPIOB, 19);

   
    
    // Loop infinito
    for (;;)
    {
        // O programa poderia alterar o duty cycle dinamicamente aqui se desejado
        pwm_tpm_CnV(TPM2, green, duty_75);
        pwm_tpm_CnV(TPM2, red, duty_50);
        k_msleep(SLEEP_TIME_MS);
         // Define o valor do duty cycle: nesse caso, duty_100 (LED quase desligado)
        pwm_tpm_CnV(TPM2, green, duty_100);
        pwm_tpm_CnV(TPM2, red, duty_100);
        k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}