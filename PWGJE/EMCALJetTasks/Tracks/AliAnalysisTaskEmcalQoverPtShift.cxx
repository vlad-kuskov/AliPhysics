/************************************************************************************
 * Copyright (C) 2020, Copyright Holders of the ALICE Collaboration                 *
 * All rights reserved.                                                             *
 *                                                                                  *
 * Redistribution and use in source and binary forms, with or without               *
 * modification, are permitted provided that the following conditions are met:      *
 *     * Redistributions of source code must retain the above copyright             *
 *       notice, this list of conditions and the following disclaimer.              *
 *     * Redistributions in binary form must reproduce the above copyright          *
 *       notice, this list of conditions and the following disclaimer in the        *
 *       documentation and/or other materials provided with the distribution.       *
 *     * Neither the name of the <organization> nor the                             *
 *       names of its contributors may be used to endorse or promote products       *
 *       derived from this software without specific prior written permission.      *
 *                                                                                  *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND  *
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED    *
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
 * DISCLAIMED. IN NO EVENT SHALL ALICE COLLABORATION BE LIABLE FOR ANY              *
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES       *
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;     *
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND      *
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT       *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS    *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                     *
 ************************************************************************************/
#include <array>
#include <iostream>
#include <string>
#include <sstream>
#include "THashList.h"
#include "THistManager.h"
#include "AliAnalysisManager.h"
#include "AliAnalysisTaskEmcalQoverPtShift.h"
#include "AliAODInputHandler.h"
#include "AliESDInputHandler.h"
#include "AliLog.h"
#include "AliTrackContainer.h"
#include "AliVEvent.h"
#include "AliVEventHandler.h"
#include "AliVTrack.h"
 
ClassImp(PWGJE::EMCALJetTasks::AliAnalysisTaskEmcalQoverPtShift)

using namespace PWGJE::EMCALJetTasks;

AliAnalysisTaskEmcalQoverPtShift::AliAnalysisTaskEmcalQoverPtShift() :
  AliAnalysisTaskEmcalLight(),
  fHistos(nullptr),
  fQOverPtShift(0.),
  fTriggerBits(0),
  fTriggerString(),
  fQoverPtShiftScanBins(100),
  fQoverPtShiftScanMin(-1e-3),
  fQoverPtShiftScanMax(1e-3),
  fQoverPtShiftScanBinCenters()
{
  SetNeedEmcalGeom(true);
}

AliAnalysisTaskEmcalQoverPtShift::AliAnalysisTaskEmcalQoverPtShift(const char *name):
  AliAnalysisTaskEmcalLight(name, kTRUE),
  fHistos(nullptr),
  fQOverPtShift(0.),
  fTriggerBits(0),
  fTriggerString(),
  fQoverPtShiftScanBins(100),
  fQoverPtShiftScanMin(-1e-3),
  fQoverPtShiftScanMax(1e-3),
  fQoverPtShiftScanBinCenters()
{
  SetNeedEmcalGeom(true);
}

AliAnalysisTaskEmcalQoverPtShift::~AliAnalysisTaskEmcalQoverPtShift()
{
}

void AliAnalysisTaskEmcalQoverPtShift::UserCreateOutputObjects() {
  AliAnalysisTaskEmcalLight::UserCreateOutputObjects();

  fHistos = new THistManager("fHistos");

  std::array<std::string, 2> chargetypes = {{"pos", "neg"}};
  for(auto charge : chargetypes) {
    fHistos->CreateTH2(Form("fHistShift%s", charge.data()), Form("Pt-shift for charge %s; p_{t}^{orig} (GeV/c); p_{t}^{shift}", charge.data()), 300, 0., 300, 300, 0., 300.);  
    fHistos->CreateTH2(Form("fHistShift%sGlobal", charge.data()), Form("Pt-shift for charge %s (glob. tracks); p_{t}^{orig} (GeV/c); p_{t}^{shift}", charge.data()), 300, 0., 300, 300, 0., 300.);  
    fHistos->CreateTH2(Form("fHistShift%sComplementary", charge.data()), Form("Pt-shift for charge %s (comp. tracks); p_{t}^{orig} (GeV/c); p_{t}^{shift}", charge.data()), 300, 0., 300, 300, 0., 300.);  
    fHistos->CreateTH2(Form("fHistScanShift%s", charge.data()), Form("%s charged particle spectrum as function of q/pt shift; q/pt shift; pt", charge.data()), fQoverPtShiftScanBins, fQoverPtShiftScanMin, fQoverPtShiftScanMax, 300, 0., 300);
    fHistos->CreateTH2(Form("fHistScanShift%sGlobal", charge.data()), Form("%s charged particle spectrum as function of q/pt shift (glob. tracks); q/pt shift; pt", charge.data()), fQoverPtShiftScanBins, fQoverPtShiftScanMin, fQoverPtShiftScanMax, 300, 0., 300);
    fHistos->CreateTH2(Form("fHistScanShift%sComplementary", charge.data()), Form("%s charged particle spectrum as function of q/pt shift (comp. tracks); q/pt shift; pt", charge.data()), fQoverPtShiftScanBins, fQoverPtShiftScanMin, fQoverPtShiftScanMax, 300, 0., 300);
    fHistos->CreateTH2(Form("fHistShift%sEMCAL", charge.data()), Form("Pt-shift for charge %s (EMCAL acceptance); p_{t}^{orig} (GeV/c); p_{t}^{shift}", charge.data()), 300, 0., 300, 300, 0., 300.);  
    fHistos->CreateTH2(Form("fHistShift%sGlobalEMCAL", charge.data()), Form("Pt-shift for charge %s (glob. tracks, EMCAL acceptance); p_{t}^{orig} (GeV/c); p_{t}^{shift}", charge.data()), 300, 0., 300, 300, 0., 300.);  
    fHistos->CreateTH2(Form("fHistShift%sComplementaryEMCAL", charge.data()), Form("Pt-shift for charge %s (comp. tracks, EMCAL acceptance); p_{t}^{orig} (GeV/c); p_{t}^{shift}", charge.data()), 300, 0., 300, 300, 0., 300.);  
    fHistos->CreateTH2(Form("fHistScanShift%sEMCAL", charge.data()), Form("%s charged particle spectrum as function of q/pt shift (EMCAL acceptance); q/pt shift; pt", charge.data()), fQoverPtShiftScanBins, fQoverPtShiftScanMin, fQoverPtShiftScanMax, 300, 0., 300);
    fHistos->CreateTH2(Form("fHistScanShift%sGlobalEMCAL", charge.data()), Form("%s charged particle spectrum as function of q/pt shift (glob. tracks, EMCAL acceptance); q/pt shift; pt", charge.data()), fQoverPtShiftScanBins, fQoverPtShiftScanMin, fQoverPtShiftScanMax, 300, 0., 300);
    fHistos->CreateTH2(Form("fHistScanShift%sComplementaryEMCAL", charge.data()), Form("%s charged particle spectrum as function of q/pt shift (comp. tracks, EMCAL acceptance); q/pt shift; pt", charge.data()), fQoverPtShiftScanBins, fQoverPtShiftScanMin, fQoverPtShiftScanMax, 300, 0., 300);
  }

  fQoverPtShiftScanBinCenters.clear();
  auto hist = static_cast<TH2 *>(fHistos->GetListOfHistograms()->FindObject("fHistScanShiftpos"));
  for(int ib = 0; ib < hist->GetXaxis()->GetNbins(); ib++){
    fQoverPtShiftScanBinCenters.push_back(hist->GetXaxis()->GetBinCenter(ib+1));
  }
  
  for(auto hist : *fHistos->GetListOfHistograms()) fOutput->Add(hist);

  PostData(1, fOutput);
}

Bool_t AliAnalysisTaskEmcalQoverPtShift::Run() {
  auto tracks = GetTrackContainer("detector");
  if(!tracks) {
    AliErrorStream() << "No track container attached, not entering track loop" << std::endl;
    return kFALSE;
  }

  TH2 *histScanPos = static_cast<TH2 *>(fHistos->GetListOfHistograms()->FindObject("fHistScanShiftpos")),
      *histScanNeg = static_cast<TH2 *>(fHistos->GetListOfHistograms()->FindObject("fHistScanShiftneg")),
      *histScanPosEMCAL = static_cast<TH2 *>(fHistos->GetListOfHistograms()->FindObject("fHistScanShiftposEMCAL")),
      *histScanNegEMCAL = static_cast<TH2 *>(fHistos->GetListOfHistograms()->FindObject("fHistScanShiftnegEMCAL")),
      *histScanPosGlobal = static_cast<TH2 *>(fHistos->GetListOfHistograms()->FindObject("fHistScanShiftposGlobal")),
      *histScanNegGlobal = static_cast<TH2 *>(fHistos->GetListOfHistograms()->FindObject("fHistScanShiftnegGlobal")),
      *histScanPosEMCALGlobal = static_cast<TH2 *>(fHistos->GetListOfHistograms()->FindObject("fHistScanShiftposGlobalEMCAL")),
      *histScanNegEMCALGlobal = static_cast<TH2 *>(fHistos->GetListOfHistograms()->FindObject("fHistScanShiftnegGlobalEMCAL")),
      *histScanPosComplementary = static_cast<TH2 *>(fHistos->GetListOfHistograms()->FindObject("fHistScanShiftposComplementary")),
      *histScanNegComplementary = static_cast<TH2 *>(fHistos->GetListOfHistograms()->FindObject("fHistScanShiftnegComplementary")),
      *histScanPosEMCALComplementary = static_cast<TH2 *>(fHistos->GetListOfHistograms()->FindObject("fHistScanShiftposComplementaryEMCAL")),
      *histScanNegEMCALComplementary = static_cast<TH2 *>(fHistos->GetListOfHistograms()->FindObject("fHistScanShiftnegComplementaryEMCAL"));
  auto trackiter = tracks->accepted();
  for(auto trkit = trackiter.begin(); trkit != trackiter.end(); ++trkit) {
    auto trk = *trkit;
    auto tracktype = tracks->GetTrackType(trkit.current_index());
    bool globaltrack = tracktype == 0, 
         comptrack = tracktype == 1;
    Double_t chargeval = trk->Charge() > 0 ? 1 : -1;
    std::string chargestring = chargeval > 0 ? "pos" : "neg";
    bool emcalacceptance = std::abs(trk->Eta()) < 0.7 && trk->Phi() >= (fGeom->GetArm1PhiMin() * TMath::DegToRad()) && trk->Phi() <= (fGeom->GetEMCALPhiMax() * TMath::DegToRad());
    Double_t ptorig = abs(trk->Pt());
    Double_t ptshift = getShiftedPt(ptorig, fQOverPtShift, chargeval);
    fHistos->FillTH2(Form("fHistShift%s", chargestring.data()), ptorig, ptshift);
    if(globaltrack) {
      fHistos->FillTH2(Form("fHistShift%sGlobal", chargestring.data()), ptorig, ptshift);
    }
    if(comptrack) {
      fHistos->FillTH2(Form("fHistShift%sComplementary", chargestring.data()), ptorig, ptshift);
    }
    if(emcalacceptance) {
      fHistos->FillTH2(Form("fHistShift%sEMCAL", chargestring.data()), ptorig, ptshift);
      if(globaltrack) {
        fHistos->FillTH2(Form("fHistShift%sGlobalEMCAL", chargestring.data()), ptorig, ptshift);
      }
      if(comptrack) {
        fHistos->FillTH2(Form("fHistShift%sComplementaryEMCAL", chargestring.data()), ptorig, ptshift);
      }
    }
    auto scanhist = chargeval > 0 ? histScanPos : histScanNeg,
         scanhistEMCAL = chargeval > 0 ? histScanPosEMCAL : histScanNegEMCAL,
         scanhistGlobal = chargeval > 0 ? histScanPosGlobal : histScanNegGlobal,
         scanhistComplementary = chargeval > 0 ? histScanPosComplementary : histScanNegComplementary,
         scanhistEMCALGlobal = chargeval > 0 ? histScanPosEMCALGlobal : histScanNegEMCALGlobal,
         scanhistEMCALComplementary = chargeval > 0 ? histScanPosEMCALComplementary : histScanNegEMCALComplementary;
    for(auto currentshift : fQoverPtShiftScanBinCenters) {
      scanhist->Fill(currentshift, getShiftedPt(ptorig, currentshift, chargeval));
      if(globaltrack) {
        scanhistGlobal->Fill(currentshift, getShiftedPt(ptorig, currentshift, chargeval));
      }
      if(comptrack) {
        scanhistComplementary->Fill(currentshift, getShiftedPt(ptorig, currentshift, chargeval));
      }
      if(emcalacceptance) {
        scanhistEMCAL->Fill(currentshift, getShiftedPt(ptorig, currentshift, chargeval));
        if(globaltrack) {
          scanhistEMCALGlobal->Fill(currentshift, getShiftedPt(ptorig, currentshift, chargeval));
        }
        if(comptrack) {
          scanhistEMCALComplementary->Fill(currentshift, getShiftedPt(ptorig, currentshift, chargeval));
        }
      }
    }
  }
  return kTRUE;
}

double AliAnalysisTaskEmcalQoverPtShift::getShiftedPt(double ptorig, double qptshift, double chargeval) {
  return 1./(qptshift*chargeval + 1./ptorig);
}

Bool_t AliAnalysisTaskEmcalQoverPtShift::IsTriggerSelected() {
  if(!(fInputHandler->IsEventSelected() & fTriggerBits)) return kFALSE;  
  if(!fInputEvent->GetFiredTriggerClasses().Contains(fTriggerString)) return kFALSE;
  return kTRUE;
}

AliAnalysisTaskEmcalQoverPtShift *AliAnalysisTaskEmcalQoverPtShift::AddTaskQOverPtShift(const char *trigger, double shift) {
  auto mgr = AliAnalysisManager::GetAnalysisManager();
  if(!mgr) {
    std::cerr << "Analysis Manager not initialized" << std::endl;
    return nullptr;
  }

  std::string track_name;
  auto inputhandler = mgr->GetInputEventHandler();
  if(inputhandler->InheritsFrom(AliAODInputHandler::Class())) {
    track_name = "tracks"; 
    std::cout << "Setting track container \"" << track_name << "\" for AODs" << std::endl;
  } else if(inputhandler->InheritsFrom(AliESDInputHandler::Class())) {
    track_name = "Tracks"; 
    std::cout << "Setting track container \"" << track_name << "\" for ESDs" << std::endl;
  } else {
    std::cerr << "Unknown input format or input handler not set" << std::endl;
    return nullptr;
  }

  UInt_t triggerbits(0);
  EMCAL_STRINGVIEW triggerstring(trigger);
  if(triggerstring == "INT7") triggerbits = AliVEvent::kINT7;
  else if(triggerstring == "EJ1" || triggerstring == "EJ2") triggerbits = AliVEvent::kEMCEJE;
  else if(triggerstring == "EG1" || triggerstring == "EG2") triggerbits = AliVEvent::kEMCEGA;
  if(!triggerbits) {
    std::cerr << "Unknown trigger class " << triggerstring << ", cannot configure task" << std::endl;
    return nullptr;
  }

  std::stringstream taskname;
  std::string qoverptstring(Form("%c%05d", shift > 0 ? 'p' : 'm', std::abs(int(shift * 1e5))));
  taskname << "QOverPtTask_" <<  qoverptstring << "_" << trigger;
  auto task = new AliAnalysisTaskEmcalQoverPtShift(taskname.str().data());
  task->AddParticleContainer(track_name.data(), "detector");
  task->SetQOverPtShift(shift);
  task->SetTriggerSelection(triggerbits, trigger);
  mgr->AddTask(task);

  std::stringstream outputfile, listname;
  outputfile << mgr->GetCommonFileName() << ":QOverPtShift_" << qoverptstring << "_" << trigger;
  listname << "QOverPtShiftHistos_" << qoverptstring << "_" << trigger;

  mgr->ConnectInput(task, 0, mgr->GetCommonInputContainer());
  mgr->ConnectOutput(task, 1, mgr->CreateContainer(listname.str().data(), TList::Class(), AliAnalysisManager::kOutputContainer, outputfile.str().data()));

  return task;
}