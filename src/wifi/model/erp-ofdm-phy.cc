/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2020 Orange Labs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Rediet <getachew.redieteab@orange.com>
 *          Sébastien Deronne <sebastien.deronne@gmail.com> (for logic ported from wifi-phy)
 *          Mathieu Lacage <mathieu.lacage@sophia.inria.fr> (for logic ported from wifi-phy)
 */

#include "erp-ofdm-phy.h"
#include "erp-ofdm-ppdu.h"
#include "wifi-psdu.h"
#include "wifi-phy.h" //only used for static mode constructor
#include "ns3/log.h"
#include "ns3/assert.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("ErpOfdmPhy");

/*******************************************************
 *       ERP-OFDM PHY (IEEE 802.11-2016, clause 18)
 *******************************************************/

/* *NS_CHECK_STYLE_OFF* */
const PhyEntity::ModulationLookupTable ErpOfdmPhy::m_erpOfdmModulationLookupTable {
  // Unique name           Code rate           Constellation size
  { "ErpOfdmRate6Mbps",  { WIFI_CODE_RATE_1_2, 2 } },
  { "ErpOfdmRate9Mbps",  { WIFI_CODE_RATE_3_4, 2 } },
  { "ErpOfdmRate12Mbps", { WIFI_CODE_RATE_1_2, 4 } },
  { "ErpOfdmRate18Mbps", { WIFI_CODE_RATE_3_4, 4 } },
  { "ErpOfdmRate24Mbps", { WIFI_CODE_RATE_1_2, 16 } },
  { "ErpOfdmRate36Mbps", { WIFI_CODE_RATE_3_4, 16 } },
  { "ErpOfdmRate48Mbps", { WIFI_CODE_RATE_2_3, 64 } },
  { "ErpOfdmRate54Mbps", { WIFI_CODE_RATE_3_4, 64 } }
};
/* *NS_CHECK_STYLE_ON* */

ErpOfdmPhy::ErpOfdmPhy ()
  : OfdmPhy (OFDM_PHY_DEFAULT, false) //don't add OFDM modes to list
{
  NS_LOG_FUNCTION (this);
  for (const auto & rate : GetErpOfdmRatesBpsList ())
    {
      WifiMode mode = GetErpOfdmRate (rate);
      NS_LOG_LOGIC ("Add " << mode << " to list");
      m_modeList.emplace_back (mode);
    }
}

ErpOfdmPhy::~ErpOfdmPhy ()
{
  NS_LOG_FUNCTION (this);
}

WifiMode
ErpOfdmPhy::GetHeaderMode (WifiTxVector txVector) const
{
  NS_ASSERT (txVector.GetMode ().GetModulationClass () == WIFI_MOD_CLASS_ERP_OFDM);
  return GetErpOfdmRate6Mbps ();
}

Time
ErpOfdmPhy::GetPreambleDuration (WifiTxVector /* txVector */) const
{
  return MicroSeconds (16); //L-STF + L-LTF
}

Time
ErpOfdmPhy::GetHeaderDuration (WifiTxVector /* txVector */) const
{
  return MicroSeconds (4); //L-SIG
}

Ptr<WifiPpdu>
ErpOfdmPhy::BuildPpdu (const WifiConstPsduMap & psdus, WifiTxVector txVector, Time /* ppduDuration */)
{
  NS_LOG_FUNCTION (this << psdus << txVector);
  return Create<ErpOfdmPpdu> (psdus.begin ()->second, txVector, m_wifiPhy->GetPhyBand (),
                              ObtainNextUid (txVector));
}

void
ErpOfdmPhy::InitializeModes (void)
{
  for (const auto & rate : GetErpOfdmRatesBpsList ())
    {
      GetErpOfdmRate (rate);
    }
}

WifiMode
ErpOfdmPhy::GetErpOfdmRate (uint64_t rate)
{
  switch (rate)
    {
      case 6000000:
        return GetErpOfdmRate6Mbps ();
      case 9000000:
        return GetErpOfdmRate9Mbps ();
      case 12000000:
        return GetErpOfdmRate12Mbps ();
      case 18000000:
        return GetErpOfdmRate18Mbps ();
      case 24000000:
        return GetErpOfdmRate24Mbps ();
      case 36000000:
        return GetErpOfdmRate36Mbps ();
      case 48000000:
        return GetErpOfdmRate48Mbps ();
      case 54000000:
        return GetErpOfdmRate54Mbps ();
      default:
        NS_ABORT_MSG ("Inexistent rate (" << rate << " bps) requested for ERP-OFDM");
        return WifiMode ();
    }
}

std::vector<uint64_t>
ErpOfdmPhy::GetErpOfdmRatesBpsList (void)
{
  return OfdmPhy::GetOfdmRatesBpsList ().at (20);
}

WifiMode
ErpOfdmPhy::GetErpOfdmRate6Mbps (void)
{
  static WifiMode mode =
    WifiModeFactory::CreateWifiMode ("ErpOfdmRate6Mbps",
                                     WIFI_MOD_CLASS_ERP_OFDM,
                                     true,
                                     WIFI_CODE_RATE_1_2,
                                     2);
  return mode;
}

WifiMode
ErpOfdmPhy::GetErpOfdmRate9Mbps (void)
{
  static WifiMode mode =
    WifiModeFactory::CreateWifiMode ("ErpOfdmRate9Mbps",
                                     WIFI_MOD_CLASS_ERP_OFDM,
                                     false,
                                     WIFI_CODE_RATE_3_4,
                                     2);
  return mode;
}

WifiMode
ErpOfdmPhy::GetErpOfdmRate12Mbps (void)
{
  static WifiMode mode =
    WifiModeFactory::CreateWifiMode ("ErpOfdmRate12Mbps",
                                     WIFI_MOD_CLASS_ERP_OFDM,
                                     true,
                                     WIFI_CODE_RATE_1_2,
                                     4);
  return mode;
}

WifiMode
ErpOfdmPhy::GetErpOfdmRate18Mbps (void)
{
  static WifiMode mode =
    WifiModeFactory::CreateWifiMode ("ErpOfdmRate18Mbps",
                                     WIFI_MOD_CLASS_ERP_OFDM,
                                     false,
                                     WIFI_CODE_RATE_3_4,
                                     4);
  return mode;
}

WifiMode
ErpOfdmPhy::GetErpOfdmRate24Mbps (void)
{
  static WifiMode mode =
    WifiModeFactory::CreateWifiMode ("ErpOfdmRate24Mbps",
                                     WIFI_MOD_CLASS_ERP_OFDM,
                                     true,
                                     WIFI_CODE_RATE_1_2,
                                     16);
  return mode;
}

WifiMode
ErpOfdmPhy::GetErpOfdmRate36Mbps (void)
{
  static WifiMode mode =
    WifiModeFactory::CreateWifiMode ("ErpOfdmRate36Mbps",
                                     WIFI_MOD_CLASS_ERP_OFDM,
                                     false,
                                     WIFI_CODE_RATE_3_4,
                                     16);
  return mode;
}

WifiMode
ErpOfdmPhy::GetErpOfdmRate48Mbps (void)
{
  static WifiMode mode =
    WifiModeFactory::CreateWifiMode ("ErpOfdmRate48Mbps",
                                     WIFI_MOD_CLASS_ERP_OFDM,
                                     false,
                                     WIFI_CODE_RATE_2_3,
                                     64);
  return mode;
}

WifiMode
ErpOfdmPhy::GetErpOfdmRate54Mbps (void)
{
  static WifiMode mode =
    WifiModeFactory::CreateWifiMode ("ErpOfdmRate54Mbps",
                                     WIFI_MOD_CLASS_ERP_OFDM,
                                     false,
                                     WIFI_CODE_RATE_3_4,
                                     64);
  return mode;
}

WifiCodeRate
ErpOfdmPhy::GetCodeRate (const std::string& name)
{
  return m_erpOfdmModulationLookupTable.at (name).first;
}

uint16_t
ErpOfdmPhy::GetConstellationSize (const std::string& name)
{
  return m_erpOfdmModulationLookupTable.at (name).second;
}

uint64_t
ErpOfdmPhy::GetPhyRate (const std::string& name, uint16_t channelWidth, uint16_t guardInterval, uint8_t nss)
{
  WifiCodeRate codeRate = GetCodeRate (name);
  uint16_t constellationSize = GetConstellationSize (name);
  uint64_t dataRate = OfdmPhy::CalculateDataRate (codeRate, constellationSize, channelWidth, guardInterval, nss);
  return OfdmPhy::CalculatePhyRate (codeRate, dataRate);
}

uint64_t
ErpOfdmPhy::GetDataRateFromTxVector (WifiTxVector txVector, uint16_t /* staId */)
{
  return GetDataRate (txVector.GetMode ().GetUniqueName (),
                      txVector.GetChannelWidth (),
                      txVector.GetGuardInterval (),
                      txVector.GetNss ());
}

uint64_t
ErpOfdmPhy::GetDataRate (const std::string& name, uint16_t channelWidth, uint16_t guardInterval, uint8_t nss)
{
  WifiCodeRate codeRate = GetCodeRate (name);
  uint16_t constellationSize = GetConstellationSize (name);
  return OfdmPhy::CalculateDataRate (codeRate, constellationSize, channelWidth, guardInterval, nss);
}

bool
ErpOfdmPhy::IsModeAllowed (uint16_t /* channelWidth */, uint8_t /* nss */)
{
  return true;
}

} //namespace ns3

namespace {

/**
 * Constructor class for ERP-OFDM modes
 */
static class ConstructorErpOfdm
{
public:
  ConstructorErpOfdm ()
  {
    ns3::ErpOfdmPhy::InitializeModes ();
    ns3::WifiPhy::AddStaticPhyEntity (ns3::WIFI_MOD_CLASS_ERP_OFDM, ns3::Create<ns3::ErpOfdmPhy> ());
  }
} g_constructor_erp_ofdm; ///< the constructor for ERP-OFDM modes

}