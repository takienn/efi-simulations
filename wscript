## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def build(bld):
    obj = bld.create_ns3_program('wifi-survive', ['applications','topology-reader', 'propagation','olsr','netanim', 'internet', 'mobility', 'wifi'])
    obj.source = ['wifi-survive.cc',
                  'efi-topology-reader.cc',
		  'nodespec.cc',
		  'experiment.cc',
		  'psr-error-model.cc']
