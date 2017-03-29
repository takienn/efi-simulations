## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def build(bld):
	obj = bld.create_ns3_program('wifi-survive', ['applications','topology-read', 'propagation','olsr','netanim', 'internet', 'mobility', 'wifi', 'flow-monitor'])
	obj.source = ['wifi-survive.cc', 'nodespec.cc', 'psr-error-model.cc', 'efi-topology-reader.cc', 'experiment.cc']
