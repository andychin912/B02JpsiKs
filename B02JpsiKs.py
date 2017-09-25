#The directories might need to be modified
from basf2 import *
from modularAnalysis import *
from variables import variables

variables.addAlias('JpsiDMID','daughter(0,extraInfo(decayModeID))')
variables.addAlias('Jpsieid','daughter(0,eid)')
variables.addAlias('Jpsimuid','daughter(0,muid)')
variables.addAlias('myRank','extraInfo(myRank)')

inputMdstList('default',['~/B02JpsiKs/B02Jpsi_ee_Ks_mdst.root','~/B02JpsiKs/B02Jpsi_mumu_Ks_mdst.root'])

fillParticleList('mu+:all','')
fillParticleList('e+:all','')
fillParticleList('K_S0:mdst','')

reconstructDecay('J/psi:allee -> e+:all e-:all','2.7<M<3.2',1)
reconstructDecay('J/psi:allmumu -> mu+:all mu-:all','2.7<M<3.2',2)

cutAndCopyList('J/psi:goodee','J/psi:allee','formula(daughter(0,eid)+daughter(1,eid))>0.1')
cutAndCopyList('J/psi:goodmumu','J/psi:allmumu','formula(daughter(0,muid)+daughter(1,muid))>0.1')
copyLists('J/psi:good',['J/psi:goodee','J/psi:goodmumu'])

reconstructDecay('B0:jpsiks -> J/psi:good K_S0:mdst', 'Mbc > 5.2 and abs(deltaE) < 0.250')

rankByLowest('B0:jpsiks','formula(abs(daughter(0,SigM))+abs(daughter(1,SigM)))',1,'myRank')
matchMCTruth('B0:jpsiks')
set_log_level(LogLevel.ERROR)



ntupleFile('B02JpsiKs_output2.root')

toolsB = ['EventMetaData', '^B0']
toolsB += ['DeltaEMbc','^B0']
toolsB += ['InvMass','B0 -> ^J/psi ^K_S0']
toolsB += ['CustomFloats[JpsiDMID:myRank]','^B0 -> J/psi K_S0']
toolsB += ['CustomFloats[Jpsieid]','B0 -> ^J/psi K_S0']
toolsB += ['CustomFloats[Jpsimuid]','B0 -> ^J/psi K_S0' ]
toolsB += ['CustomFloats[isSignal]','^B0 -> ^J/psi K_S0']
toolsB += ['MCTruth','^B0 -> ^J/psi ^K_S0']

ntupleTree('h1','B0:jpsiks',toolsB)

process(analysis_main)


