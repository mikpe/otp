%%
%% %CopyrightBegin%
%%
%% SPDX-License-Identifier: Apache-2.0
%%
%% Copyright Ericsson AB 2008-2025. All Rights Reserved.
%%
%% Licensed under the Apache License, Version 2.0 (the "License");
%% you may not use this file except in compliance with the License.
%% You may obtain a copy of the License at
%%
%%     http://www.apache.org/licenses/LICENSE-2.0
%%
%% Unless required by applicable law or agreed to in writing, software
%% distributed under the License is distributed on an "AS IS" BASIS,
%% WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
%% See the License for the specific language governing permissions and
%% limitations under the License.
%%
%% %CopyrightEnd%
%% This file is generated DO NOT EDIT

-module(wxWindowCreateEvent).
-moduledoc """
This event is sent just after the actual window associated with a `m:wxWindow` object has
been created.

Since it is derived from `m:wxCommandEvent`, the event propagates up the window hierarchy.

See:
* [Overview events](https://docs.wxwidgets.org/3.2/overview_events.html#overview_events)

* `m:wxWindowDestroyEvent`

This class is derived, and can use functions, from:

* `m:wxCommandEvent`

* `m:wxEvent`

wxWidgets docs: [wxWindowCreateEvent](https://docs.wxwidgets.org/3.2/classwx_window_create_event.html)

## Events

Use `wxEvtHandler:connect/3` with `wxWindowCreateEventType` to subscribe to events of this type.
""".
-include("wxe.hrl").
-export([]).

%% inherited exports
-export([getClientData/1,getExtraLong/1,getId/1,getInt/1,getSelection/1,getSkipped/1,
  getString/1,getTimestamp/1,isChecked/1,isCommandEvent/1,isSelection/1,
  parent_class/1,resumePropagation/2,setInt/2,setString/2,shouldPropagate/1,
  skip/1,skip/2,stopPropagation/1]).

-type wxWindowCreateEvent() :: wx:wx_object().
-include("wx.hrl").
-type wxWindowCreateEventType() :: 'create'.
-export_type([wxWindowCreateEvent/0, wxWindowCreate/0, wxWindowCreateEventType/0]).
-doc false.
parent_class(wxCommandEvent) -> true;
parent_class(wxEvent) -> true;
parent_class(_Class) -> erlang:error({badtype, ?MODULE}).

 %% From wxCommandEvent
-doc false.
setString(This,String) -> wxCommandEvent:setString(This,String).
-doc false.
setInt(This,IntCommand) -> wxCommandEvent:setInt(This,IntCommand).
-doc false.
isSelection(This) -> wxCommandEvent:isSelection(This).
-doc false.
isChecked(This) -> wxCommandEvent:isChecked(This).
-doc false.
getString(This) -> wxCommandEvent:getString(This).
-doc false.
getSelection(This) -> wxCommandEvent:getSelection(This).
-doc false.
getInt(This) -> wxCommandEvent:getInt(This).
-doc false.
getExtraLong(This) -> wxCommandEvent:getExtraLong(This).
-doc false.
getClientData(This) -> wxCommandEvent:getClientData(This).
 %% From wxEvent
-doc false.
stopPropagation(This) -> wxEvent:stopPropagation(This).
-doc false.
skip(This, Options) -> wxEvent:skip(This, Options).
-doc false.
skip(This) -> wxEvent:skip(This).
-doc false.
shouldPropagate(This) -> wxEvent:shouldPropagate(This).
-doc false.
resumePropagation(This,PropagationLevel) -> wxEvent:resumePropagation(This,PropagationLevel).
-doc false.
isCommandEvent(This) -> wxEvent:isCommandEvent(This).
-doc false.
getTimestamp(This) -> wxEvent:getTimestamp(This).
-doc false.
getSkipped(This) -> wxEvent:getSkipped(This).
-doc false.
getId(This) -> wxEvent:getId(This).
