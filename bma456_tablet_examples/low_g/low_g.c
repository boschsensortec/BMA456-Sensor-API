/**\
 * Copyright (c) 2023 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#include <stdio.h>
#include "bma456_tablet.h"
#include "common.h"

/******************************************************************************/
/*!            Function                                                       */

/* This function starts the execution of program. */
int main(void)
{
    /* Variable to store the status of API */
    int8_t rslt;

    /* Sensor initialization configuration */
    struct bma4_dev bma = { 0 };

    /* Variable to store step activity interrupt status */
    uint16_t int_status = 0;

    struct bma456_tablet_low_g_config low_g = { 0 };
    struct bma456_tablet_low_g_config get_low_g = { 0 };

    /* Interface reference is given as a parameter
     *         For I2C : BMA4_I2C_INTF
     *         For SPI : BMA4_SPI_INTF
     * Variant information given as parameter - BMA45X_VARIANT
     */
    rslt = bma4_interface_init(&bma, BMA4_I2C_INTF, BMA45X_VARIANT);
    bma4_error_codes_print_result("bma4_interface_init", rslt);

    /* Sensor initialization */
    rslt = bma456_tablet_init(&bma);
    bma4_error_codes_print_result("bma456_tablet_init status", rslt);

    /* Upload the configuration file to enable the features of the sensor. */
    rslt = bma456_tablet_write_config_file(&bma);
    bma4_error_codes_print_result("bma456_tablet_write_config status", rslt);

    /* Enable the accelerometer */
    rslt = bma4_set_accel_enable(BMA4_ENABLE, &bma);
    bma4_error_codes_print_result("bma4_set_accel_enable status", rslt);

    low_g.threshold = 0x133;
    low_g.duration = 0x0;
    low_g.hysteresis = 0x100;

    rslt = bma456_tablet_set_low_g_config(&low_g, &bma);
    bma4_error_codes_print_result("bma456_tablet_feature_enable status", rslt);

    rslt = bma456_tablet_get_low_g_config(&get_low_g, &bma);
    bma4_error_codes_print_result("bma456_tablet_feature_enable status", rslt);

    printf("Low-g Threshold : 0x%x\n\n", get_low_g.threshold);

    /* Enable low-g feature */
    rslt = bma456_tablet_feature_enable(BMA456_TABLET_LOW_G, 1, &bma);
    bma4_error_codes_print_result("bma456_tablet_feature_enable status", rslt);

    if (rslt == BMA4_OK)
    {
        /* Map the interrupt 1 for low-g detection */
        rslt = bma456_tablet_map_interrupt(BMA4_INTR1_MAP, BMA456_TABLET_LOW_G_INT, BMA4_ENABLE, &bma);
        bma4_error_codes_print_result("bma456_tablet_map_interrupt status", rslt);

        if (rslt == BMA4_OK)
        {
            printf("Drop the board for low-g\n");

            for (;;)
            {
                /* Read interrupt status */
                rslt = bma456_tablet_read_int_status(&int_status, &bma);
                bma4_error_codes_print_result("bma456_tablet_read_int_status", rslt);

                /* Filtering only the low-g interrupt */
                if ((rslt == BMA4_OK) && (int_status & BMA456_TABLET_LOW_G_INT))
                {
                    printf("\nLow-g interrupt occurred\n");
                    break;
                }

                int_status = 0;
            }
        }
    }

    bma4_coines_deinit();

    return rslt;
}
