#!/usr/bin/env python3
"""Compile the actual resident-observer API and verify default-null behavior."""
from pathlib import Path
import subprocess,tempfile,os
r=Path(__file__).resolve().parents[1];s=(r/'NativeGOPPresenter.cpp').read_text()
observer=s[s.index('using NativeGOPLatencyObserver'):s.index('class NativeGOPClient;')]
source=r'''
#include <cstdint>
#include <cassert>
static unsigned ticks=0,calls=0;
static uint64_t mach_absolute_time(){return ++ticks;}
OBSERVER
static void observe(uint32_t kind,uint64_t id,uint64_t tick){assert(kind==30&&id==123&&tick);++calls;}
static void other(uint32_t,uint64_t,uint64_t){assert(false);}
int main(){latencyEvent(30,123);assert(!ticks&&!calls);
native_gop_set_latency_observer(observe);native_gop_set_latency_observer(other);
latencyEvent(30,123);assert(ticks==1&&calls==1);}
'''.replace('OBSERVER',observer)
with tempfile.TemporaryDirectory() as d:
 p=Path(d);(p/'test.cpp').write_text(source)
 subprocess.run([os.environ.get('CXX','c++'),'-std=c++17','-Wall','-Wextra','-Werror','-fsanitize=address,undefined',str(p/'test.cpp'),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
assert 'IOLog' not in observer and 'new ' not in observer
assert s.index('latencyEvent(32,sequence)')<s.index('native_legacy_gpu_complete(done.display,status)')<s.index('latencyEvent(33,sequence)')
print('PASS actual default-null observer, no clock when off, one-time resident registration, callback ABI and completion boundaries')
