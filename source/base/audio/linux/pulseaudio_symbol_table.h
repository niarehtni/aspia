//
// Aspia Project
// Copyright (C) 2020 Dmitry Chapyshev <dmitry@aspia.ru>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.
//

#ifndef BASE__AUDIO__LINUX__PULSEAUDIO_SYMBOL_TABLE_H
#define BASE__AUDIO__LINUX__PULSEAUDIO_SYMBOL_TABLE_H

#include "base/audio/linux/late_binding_symbol_table.h"

namespace base {

// The PulseAudio symbols we need, as an X-Macro list.
#define PULSE_AUDIO_SYMBOLS_LIST           \
    X(pa_bytes_per_second)                   \
    X(pa_context_connect)                    \
    X(pa_context_disconnect)                 \
    X(pa_context_errno)                      \
    X(pa_context_get_protocol_version)       \
    X(pa_context_get_server_info)            \
    X(pa_context_get_sink_info_list)         \
    X(pa_context_get_sink_info_by_index)     \
    X(pa_context_get_sink_info_by_name)      \
    X(pa_context_get_sink_input_info)        \
    X(pa_context_get_source_info_by_index)   \
    X(pa_context_get_source_info_by_name)    \
    X(pa_context_get_source_info_list)       \
    X(pa_context_get_state)                  \
    X(pa_context_new)                        \
    X(pa_context_set_sink_input_volume)      \
    X(pa_context_set_sink_input_mute)        \
    X(pa_context_set_source_volume_by_index) \
    X(pa_context_set_source_mute_by_index)   \
    X(pa_context_set_state_callback)         \
    X(pa_context_unref)                      \
    X(pa_cvolume_set)                        \
    X(pa_operation_get_state)                \
    X(pa_operation_unref)                    \
    X(pa_stream_connect_playback)            \
    X(pa_stream_connect_record)              \
    X(pa_stream_disconnect)                  \
    X(pa_stream_drop)                        \
    X(pa_stream_get_device_index)            \
    X(pa_stream_get_index)                   \
    X(pa_stream_get_latency)                 \
    X(pa_stream_get_sample_spec)             \
    X(pa_stream_get_state)                   \
    X(pa_stream_new)                         \
    X(pa_stream_peek)                        \
    X(pa_stream_readable_size)               \
    X(pa_stream_set_buffer_attr)             \
    X(pa_stream_set_overflow_callback)       \
    X(pa_stream_set_read_callback)           \
    X(pa_stream_set_state_callback)          \
    X(pa_stream_set_underflow_callback)      \
    X(pa_stream_set_write_callback)          \
    X(pa_stream_unref)                       \
    X(pa_stream_writable_size)               \
    X(pa_stream_write)                       \
    X(pa_strerror)                           \
    X(pa_threaded_mainloop_free)             \
    X(pa_threaded_mainloop_get_api)          \
    X(pa_threaded_mainloop_lock)             \
    X(pa_threaded_mainloop_new)              \
    X(pa_threaded_mainloop_signal)           \
    X(pa_threaded_mainloop_start)            \
    X(pa_threaded_mainloop_stop)             \
    X(pa_threaded_mainloop_unlock)           \
    X(pa_threaded_mainloop_wait)

LATE_BINDING_SYMBOL_TABLE_DECLARE_BEGIN(PulseAudioSymbolTable)
#define X(sym) LATE_BINDING_SYMBOL_TABLE_DECLARE_ENTRY(PulseAudioSymbolTable, sym)
PULSE_AUDIO_SYMBOLS_LIST
#undef X
LATE_BINDING_SYMBOL_TABLE_DECLARE_END(PulseAudioSymbolTable)

} // namespace base

#endif // BASE__AUDIO__LINUX__PULSEAUDIO_SYMBOL_TABLE_H
