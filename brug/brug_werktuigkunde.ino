/************************************************************************/
/*                                                                 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

************************************************************************/
/*
For beginners tutorial (and user guide) visit:
http://www.openelectrons.com/docs/viewdoc/25
*/

#include <Wire.h>
#include <EVShield.h>

EVShield evshield(0x34, 0x36);

void
setup()
{
    Serial.begin(115200);       // start serial for output
    delay(500);                // wait two seconds, allowing time to

    evshield.init( SH_HardwareI2C );

    Serial.println ("Press GO button to continue");
    evshield.waitForButtonPress(BTN_GO);

    evshield.bank_a.motorReset();
}

void loop() {
  if (evshield.getButtonState(BTN_LEFT)) {
    evshield.bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Forward, 100);
  }

  if (evshield.getButtonState(BTN_RIGHT)) {
    evshield.bank_a.motorRunUnlimited(SH_Motor_1, SH_Direction_Reverse, 100);
  }
}
