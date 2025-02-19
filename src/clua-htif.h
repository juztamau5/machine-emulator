// Copyright Cartesi and individual authors (see AUTHORS)
// SPDX-License-Identifier: LGPL-3.0-or-later
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along
// with this program (see COPYING). If not, see <https://www.gnu.org/licenses/>.
//

#ifndef CLUA_HTIF_H
#define CLUA_HTIF_H

#include "clua.h"

/// \file
/// \brief HTIF device Lua interface

namespace cartesi {

/// \brief Exports symbols to table on top of Lua stack
/// \param L Lua state
/// \param ctxidx Index of Clua context
int clua_htif_export(lua_State *L, int ctxidx);

} // namespace cartesi

#endif
