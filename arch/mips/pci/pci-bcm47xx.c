/*
 *  Copyright (C) 2008 Aurelien Jarno <aurelien@aurel32.net>
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <freax/types.h>
#include <freax/pci.h>
#include <freax/ssb/ssb.h>
#include <freax/bcma/bcma.h>
#include <bcm47xx.h>

int pcibios_map_irq(const struct pci_dev *dev, u8 slot, u8 pin)
{
	return 0;
}

#ifdef CONFIG_BCM47XX_SSB
static int bcm47xx_pcibios_plat_dev_init_ssb(struct pci_dev *dev)
{
	int res;
	u8 slot, pin;

	res = ssb_pcibios_plat_dev_init(dev);
	if (res < 0) {
		pci_alert(dev, "PCI: Failed to init device\n");
		return res;
	}

	pci_read_config_byte(dev, PCI_INTERRUPT_PIN, &pin);
	slot = PCI_SLOT(dev->devfn);
	res = ssb_pcibios_map_irq(dev, slot, pin);

	/* IRQ-0 and IRQ-1 are software interrupts. */
	if (res < 2) {
		pci_alert(dev, "PCI: Failed to map IRQ of device\n");
		return res;
	}

	dev->irq = res;
	return 0;
}
#endif

#ifdef CONFIG_BCM47XX_BCMA
static int bcm47xx_pcibios_plat_dev_init_bcma(struct pci_dev *dev)
{
	int res;

	res = bcma_core_pci_plat_dev_init(dev);
	if (res < 0) {
		pci_alert(dev, "PCI: Failed to init device\n");
		return res;
	}

	res = bcma_core_pci_pcibios_map_irq(dev);

	/* IRQ-0 and IRQ-1 are software interrupts. */
	if (res < 2) {
		pci_alert(dev, "PCI: Failed to map IRQ of device\n");
		return res;
	}

	dev->irq = res;
	return 0;
}
#endif

int pcibios_plat_dev_init(struct pci_dev *dev)
{
#ifdef CONFIG_BCM47XX_SSB
	if (bcm47xx_bus_type ==	 BCM47XX_BUS_TYPE_SSB)
		return bcm47xx_pcibios_plat_dev_init_ssb(dev);
#endif
#ifdef CONFIG_BCM47XX_BCMA
	if  (bcm47xx_bus_type ==  BCM47XX_BUS_TYPE_BCMA)
		return bcm47xx_pcibios_plat_dev_init_bcma(dev);
#endif
	return 0;
}
