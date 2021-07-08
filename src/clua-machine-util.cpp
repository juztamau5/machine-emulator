// Copyright 2020 Cartesi Pte. Ltd.
//
// This file is part of the machine-emulator. The machine-emulator is free
// software: you can redistribute it and/or modify it under the terms of the GNU
// Lesser General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// The machine-emulator is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with the machine-emulator. If not, see http://www.gnu.org/licenses/.
//

#include <unordered_map>
#include <cstring>

#include "clua.h"
#include "clua-machine-util.h"

namespace cartesi {

/// \brief Deleter for C string
template<>
void cm_delete<char>(char *ptr) {
    cm_delete_error_message(ptr);
}

/// \brief Deleter for C api machine configuration
template<>
void cm_delete<const cm_machine_config>(const cm_machine_config *ptr) {
    cm_delete_machine_config(ptr);
}
template<>
void cm_delete<cm_machine_config>(cm_machine_config *ptr) {
    cm_delete_machine_config(ptr);
}

/// \brief Deleter for C api runtime machine configuration
template<>
void cm_delete(cm_machine_runtime_config *ptr) {
    cm_delete_machine_runtime_config(ptr);
}

/// \brief Deleter for C api machine
template<>
void cm_delete(cm_machine *ptr) {
    cm_delete_machine(ptr);
}

/// \brief Deleter for C api access log
template<>
void cm_delete(cm_access_log *ptr) {
    cm_delete_access_log(ptr);
}

/// \brief Deleter for C api merkle tree proof
template<>
void cm_delete(cm_merkle_tree_proof *p) {
    cm_delete_proof(p);
}

using csr_map = std::unordered_map<std::string, machine::csr>;

/// \brief Mapping between CSR names and constants
static const csr_map& csr_names() {
    static const csr_map map = {
        {"pc", machine::csr::pc},
        {"mvendorid", machine::csr::mvendorid},
        {"marchid", machine::csr::marchid},
        {"mimpid", machine::csr::mimpid},
        {"mcycle", machine::csr::mcycle},
        {"minstret", machine::csr::minstret},
        {"mstatus", machine::csr::mstatus},
        {"mtvec", machine::csr::mtvec},
        {"mscratch", machine::csr::mscratch},
        {"mepc", machine::csr::mepc},
        {"mcause", machine::csr::mcause},
        {"mtval", machine::csr::mtval},
        {"misa", machine::csr::misa},
        {"mie", machine::csr::mie},
        {"mip", machine::csr::mip},
        {"medeleg", machine::csr::medeleg},
        {"mideleg", machine::csr::mideleg},
        {"mcounteren", machine::csr::mcounteren},
        {"stvec", machine::csr::stvec},
        {"sscratch", machine::csr::sscratch},
        {"sepc", machine::csr::sepc},
        {"scause", machine::csr::scause},
        {"stval", machine::csr::stval},
        {"satp", machine::csr::satp},
        {"scounteren", machine::csr::scounteren},
        {"ilrsc", machine::csr::ilrsc},
        {"iflags", machine::csr::iflags},
        {"clint_mtimecmp", machine::csr::clint_mtimecmp},
        {"htif_tohost", machine::csr::htif_tohost},
        {"htif_fromhost", machine::csr::htif_fromhost},
        {"htif_ihalt", machine::csr::htif_ihalt},
        {"htif_iconsole", machine::csr::htif_iconsole},
        {"htif_iyield", machine::csr::htif_iyield},
        {"dhd_tstart", machine::csr::dhd_tstart},
        {"dhd_tlength", machine::csr::dhd_tlength},
        {"dhd_dlength", machine::csr::dhd_dlength},
        {"dhd_hlength", machine::csr::dhd_hlength}
    };
    return map;
}

/// \brief Mapping between CSR names and C API constants
const static std::unordered_map<std::string, CM_PROC_CSR> g_cm_proc_csr_name = { // NOLINT(cert-err58-cpp)
    {"pc", CM_PROC_PC},
    {"mvendorid", CM_PROC_MVENDORID},
    {"marchid", CM_PROC_MARCHID},
    {"mimpid", CM_PROC_MIMPID},
    {"mcycle", CM_PROC_MCYCLE},
    {"minstret", CM_PROC_MINSTRET},
    {"mstatus", CM_PROC_MSTATUS},
    {"mtvec", CM_PROC_MTVEC},
    {"mscratch", CM_PROC_MSCRATCH},
    {"mepc", CM_PROC_MEPC},
    {"mcause", CM_PROC_MCAUSE},
    {"mtval", CM_PROC_MTVAL},
    {"misa", CM_PROC_MISA},
    {"mie", CM_PROC_MIE},
    {"mip", CM_PROC_MIP},
    {"medeleg", CM_PROC_MEDELEG},
    {"mideleg", CM_PROC_MIDELEG},
    {"mcounteren", CM_PROC_MCOUNTEREN},
    {"stvec", CM_PROC_STVEC},
    {"sscratch", CM_PROC_SSCRATCH},
    {"sepc", CM_PROC_SEPC},
    {"scause", CM_PROC_SCAUSE},
    {"stval", CM_PROC_STVAL},
    {"satp", CM_PROC_SATP},
    {"scounteren", CM_PROC_SCOUNTEREN},
    {"ilrsc", CM_PROC_ILRSC},
    {"iflags", CM_PROC_IFLAGS},
    {"clint_mtimecmp", CM_PROC_CLINT_MTIMECMP},
    {"htif_tohost", CM_PROC_HTIF_TOHOST},
    {"htif_fromhost", CM_PROC_HTIF_FROMHOST},
    {"htif_ihalt", CM_PROC_HTIF_IHALT},
    {"htif_iconsole", CM_PROC_HTIF_ICONSOLE},
    {"htif_iyield", CM_PROC_HTIF_IYIELD},
    {"dhd_tstart", CM_PROC_DHD_TSTART},
    {"dhd_tlength", CM_PROC_DHD_TLENGTH},
    {"dhd_dlength", CM_PROC_DHD_DLENGTH},
    {"dhd_hlength", CM_PROC_DHD_HLENGTH}
};


static char *copy_lua_str(lua_State *L, int idx) {
    const char* lua_str = lua_tostring(L, idx);
    auto size = strlen(lua_str) + 1;
    auto *copy = new char[size];
    strncpy(copy, lua_str, size);
    return copy;
}

/// \brief Returns an optional boolean field indexed by string in a table.
/// \param L Lua state.
/// \param tabidx Table stack index.
/// \param field Field index.
/// \returns Field value, or false if missing.
static bool opt_boolean_field(lua_State *L, int tabidx, const char *field) {
    tabidx = lua_absindex(L, tabidx);
    lua_getfield(L, tabidx, field);
    bool val = lua_toboolean(L, -1);
    lua_pop(L, 1);
    return val;
}

/// \brief Returns an optional integer field indexed by integer in a table.
/// \param L Lua state.
/// \param tabidx Table stack index.
/// \param field Field index.
/// \param def Default value for missing field.
/// \returns Field value, or default value if missing.
static uint64_t opt_uint_field(lua_State *L, int tabidx, int field, uint64_t def = 0) {
    tabidx = lua_absindex(L, tabidx);
    uint64_t val = def;
    lua_geti(L, tabidx, field);
    if (lua_isinteger(L, -1)) {
        val = lua_tointeger(L, -1);
    } else if (!lua_isnil(L, -1)) {
        luaL_error(L, "invalid entry %d (expected unsigned integer)", field);
    }
    lua_pop(L, 1);
    return static_cast<uint64_t>(val);
}

/// \brief Returns an optional integer field indexed by string in a table.
/// \param L Lua state.
/// \param tabidx Table stack index.
/// \param field Field index.
/// \param def Default value for missing field.
/// \returns Field value, or default value if missing.
static uint64_t opt_uint_field(lua_State *L, int tabidx, const char *field, uint64_t def = 0) {
    tabidx = lua_absindex(L, tabidx);
    uint64_t val = def;
    lua_getfield(L, tabidx, field);
    if (lua_isinteger(L, -1)) {
        val = lua_tointeger(L, -1);
    } else if (!lua_isnil(L, -1)) {
        luaL_error(L, "invalid %s (expected unsigned integer)", field);
    }
    lua_pop(L, 1);
    return static_cast<uint64_t>(val);
}

/// \brief Returns an optional string field indexed by string in a table.
/// \param L Lua state.
/// \param tabidx Table stack index.
/// \param field Field index.
/// \returns Field value, or empty string if missing.
static std::string opt_string_field(lua_State *L, int tabidx, const char *field) {
    tabidx = lua_absindex(L, tabidx);
    std::string str;
    lua_getfield(L, tabidx, field);
    if (lua_isstring(L, -1)) {
        str = lua_tostring(L, -1);
    } else if (!lua_isnil(L, -1)) {
        luaL_error(L, "invalid %s (expected string)", field);
    }
    lua_pop(L, 1);
    return str;
}

/// \brief Returns an allocated optional c string field indexed by string in a table.
/// \param L Lua state.
/// \param tabidx Table stack index.
/// \param field Field index.
/// \returns Field value, or nullptr if missing. If field value is returned,
/// it must be deallocated by the caller
static char *opt_string_field_c(lua_State *L, int tabidx, const char *field) {
    tabidx = lua_absindex(L, tabidx);
    char *str = nullptr;
    lua_getfield(L, tabidx, field);
    if (lua_isstring(L, -1)) {
        str = copy_lua_str(L, -1);
    } else if (!lua_isnil(L, -1)) {
        luaL_error(L, "invalid %s (expected string)", field);
    }
    lua_pop(L, 1);
    return str;
}

/// \brief Returns an integer field indexed by string in a table.
/// \param L Lua state.
/// \param tabidx Table stack index.
/// \param field Field index.
/// \returns Field value. Throws error if field is missing.
static int check_int_field(lua_State *L, int tabidx, const char *field) {
    tabidx = lua_absindex(L, tabidx);
    lua_getfield(L, tabidx, field);
    if (!lua_isinteger(L, -1)) {
        luaL_error(L, "invalid %s (expected integer, got %s)", field,
            lua_typename(L, lua_type(L, -1)));
    }
    lua_Integer ival = lua_tointeger(L, -1);
    lua_pop(L, 1);
    return static_cast<int>(ival);
}

/// \brief Returns an integer field indexed by string in a table.
/// \param L Lua state.
/// \param tabidx Table stack index.
/// \param field Field index.
/// \returns Field value. Throws error if field is missing.
static uint64_t check_uint_field(lua_State *L, int tabidx, const char *field) {
    tabidx = lua_absindex(L, tabidx);
    lua_getfield(L, tabidx, field);
    if (!lua_isinteger(L, -1)) {
        luaL_error(L, "invalid %s (expected unsigned integer, got %s)", field,
            lua_typename(L, lua_type(L, -1)));
    }
    lua_Integer ival = lua_tointeger(L, -1);
    lua_pop(L, 1);
    return static_cast<uint64_t>(ival);
}


/// \brief Returns a string field indexed by string in a table.
/// \param L Lua state.
/// \param tabidx Table stack index.
/// \param field Field index.
/// \returns Field value. Throws error if field is missing.
static std::string check_string_field(lua_State *L, int tabidx, const char *field) {
    tabidx = lua_absindex(L, tabidx);
    std::string str;
    lua_getfield(L, tabidx, field);
    if (lua_isstring(L, -1)) {
        str = lua_tostring(L, -1);
    } else {
        luaL_error(L, "invalid %s (expected string)", field);
    }
    lua_pop(L, 1);
    return str;
}

/// \brief Returns a allocated c string field indexed by string in a table.
/// \param L Lua state
/// \param tabidx Table stack index
/// \param field Field index
/// \returns Field value as c string. Throws error if field is missing. Returned result
/// must be deallocated from the user
static char *check_string_field_c(lua_State *L, int tabidx, const char *field) {
    tabidx = lua_absindex(L, tabidx);
    char* str = nullptr;
    lua_getfield(L, tabidx, field);
    if (lua_isstring(L, -1)) {
        str = copy_lua_str(L, -1);
    } else {
        luaL_error(L, "invalid %s (expected string)", field);
    }
    lua_pop(L, 1);
    return str;
}

/// \brief Pushes to the stack a table field indexed by string in another table.
/// \param L Lua state.
/// \param tabidx Table stack index.
/// \param field Field index.
/// \returns Field is returned in Lua stack.
static void check_table_field(lua_State *L, int tabidx, const char *field) {
    lua_getfield(L, tabidx, field);
    if (!lua_istable(L, -1)) {
        luaL_error(L, "missing %s", field);
    }
}

/// \brief Pushes to stack an optional table field indexed by string in a table.
/// \param L Lua state.
/// \param tabidx Table stack index.
/// \param field Field index.
/// \returns True if field is present, false if missing. If field is present,
/// it is pushed to the Lua stack, otherwise stack is left unchanged.
static bool opt_table_field(lua_State *L, int tabidx, const char *field) {
    lua_getfield(L, tabidx, field);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        return false;
    } else if (!lua_istable(L, -1)) {
        luaL_error(L, "missing %s", field);
        return false;
    } else {
        return true;
    }
}

/// \brief Returns an access_type table field indexed by string in a table.
/// \param L Lua state.
/// \param tabidx Table stack index.
/// \param field Field index.
/// \returns Corresponding access_type.
static access_type check_access_type_field(lua_State *L, int tabidx,
    const char *field) {
    auto name = check_string_field(L, tabidx, field);
    if (name == "read") {
        return access_type::read;
    } else if (name == "write") {
        return access_type::write;
    } else {
        luaL_error(L, "invalid %s (expected access type)", field);
        return access_type::read; // never reached
    }
}

/// \brief Returns an CM_ACCESS_TYPE table field indexed by string in a table
/// \param L Lua state
/// \param tabidx Table stack index
/// \param field Field index
/// \returns Corresponding CM_ACCESS_TYPE
static CM_ACCESS_TYPE check_cm_access_type_field(lua_State *L, int tabidx,
    const char *field) {
    auto name = check_string_field(L, tabidx, field);
    if (name == "read") {
        return CM_ACCESS_READ;
    } else if (name == "write") {
        return CM_ACCESS_WRITE;
    } else {
        luaL_error(L, "invalid %s (expected access type)", field);
        return CM_ACCESS_READ; // never reached
    }
}

/// \brief Returns an bracket_type table field indexed by string in a table.
/// \param L Lua state.
/// \param tabidx Table stack index.
/// \param field Field index.
/// \returns Corresponding bracket_type.
static bracket_type check_bracket_type_field(lua_State *L, int tabidx,
    const char *field) {
    auto name = check_string_field(L, tabidx, field);
    if (name == "begin") {
        return bracket_type::begin;
    } else if (name == "end") {
        return bracket_type::end;
    } else {
        luaL_error(L, "invalid %s (expected bracket type)", field);
        return bracket_type::begin; // never reached
    }
}

/// \brief Returns an CM_BRACKET_TYPE table field indexed by string in a table.
/// \param L Lua state
/// \param tabidx Table stack index
/// \param field Field index
/// \returns Corresponding CM_BRACKET_TYPE
static CM_BRACKET_TYPE check_cm_bracket_type_field(lua_State *L, int tabidx,
    const char *field) {
    auto name = check_string_field(L, tabidx, field);
    if (name == "begin") {
        return CM_BRACKET_BEGIN;
    } else if (name == "end") {
        return CM_BRACKET_END;
    } else {
        luaL_error(L, "invalid %s (expected bracket type)", field);
        return CM_BRACKET_BEGIN; // never reached
    }
}

/// \brief Loads a bracket_note from Lua.
/// \param L Lua state.
/// \param tabidx Bracket_note stack index.
/// \returns The bracket_note.
static bracket_note check_bracket_note(lua_State *L, int tabidx) {
    luaL_checktype(L, tabidx, LUA_TTABLE);
    return {
        check_bracket_type_field(L, -1, "type"),
        check_uint_field(L, -1, "where")-1, // confert from 1- to 0-based index
        check_string_field(L, -1, "text")
    };
}

/// \brief Loads a cm_bracket_note from Lua
/// \param L Lua state
/// \param tabidx Bracket_note stack index
/// \returns The bracket note
static cm_bracket_note check_cm_bracket_note(lua_State *L, int tabidx) {
    luaL_checktype(L, tabidx, LUA_TTABLE);
    cm_bracket_note new_bracket_note{};
    new_bracket_note.type = check_cm_bracket_type_field(L, -1, "type");
    new_bracket_note.where = check_uint_field(L, -1, "where")-1; // confert from 1- to 0-based index
    new_bracket_note.text = check_string_field_c(L, -1, "text");
    return new_bracket_note;
}

/// \brief Loads an array of sibling_hashes from Lua.
/// \param L Lua state.
/// \param idx Proof stack index.
/// \param log2_target_size of node from which to obtain siblings.
/// \param p Proof to receive sibling_hashes.
static void check_sibling_hashes(lua_State *L,
    int idx, int log2_target_size, machine_merkle_tree::proof_type &p) {
    luaL_checktype(L, idx, LUA_TTABLE);
    if (log2_target_size < 0) {
        luaL_error(L, "invalid log2_target_size");
    }
    for ( ; log2_target_size < p.get_log2_root_size(); ++log2_target_size) {
        lua_rawgeti(L, idx, p.get_log2_root_size()-log2_target_size);
        p.set_sibling_hash(clua_check_hash(L, -1), log2_target_size);
        lua_pop(L, 1);
    }
}

/// \brief Loads an array of sibling cm_hashes from Lua
/// \param L Lua state.
/// \param idx Proof stack index.
/// \param log2_target_size Size of node from which to obtain siblings
/// \param p Merkle tree proof to receive sibling_hashes
static void check_sibling_cm_hashes(lua_State *L, int idx, size_t log2_target_size,
    cm_merkle_tree_proof *p) {
    luaL_checktype(L, idx, LUA_TTABLE);
    p->sibling_hashes_count = p->log2_root_size - log2_target_size;
    p->sibling_hashes = new cm_hash[p->sibling_hashes_count];
    memset(p->sibling_hashes, 0, sizeof(cm_hash) * p->sibling_hashes_count);
    for (; log2_target_size < p->log2_root_size; ++log2_target_size) {
        lua_rawgeti(L, idx, p->log2_root_size - log2_target_size);
        auto index = p->log2_root_size - 1 - log2_target_size;
        clua_check_cm_hash(L, -1, &p->sibling_hashes[index]);
        lua_pop(L, 1);
    }
}

/// \brief Loads a proof from Lua.
/// \param L Lua state.
/// \param tabidx Proof stack index.
/// \returns The proof.
machine_merkle_tree::proof_type clua_check_proof(lua_State *L, int tabidx) {
    luaL_checktype(L, tabidx, LUA_TTABLE);
    int log2_target_size = check_uint_field(L, tabidx, "log2_target_size");
    int log2_root_size = check_uint_field(L, tabidx, "log2_root_size");
    machine_merkle_tree::proof_type proof{log2_root_size, log2_target_size};
    proof.set_target_address(check_uint_field(L, tabidx, "target_address"));
    lua_getfield(L, tabidx, "target_hash");
    proof.set_target_hash(clua_check_hash(L, -1));
    lua_pop(L, 1);
    lua_getfield(L, tabidx, "root_hash");
    proof.set_root_hash(clua_check_hash(L, -1));
    lua_pop(L, 1);
    lua_getfield(L, tabidx, "sibling_hashes");
    check_sibling_hashes(L, -1, log2_target_size, proof);
    lua_pop(L, 1);
    return proof;
}

cm_merkle_tree_proof *clua_check_cm_merkle_tree_proof(lua_State *L, int tabidx) {
    luaL_checktype(L, tabidx, LUA_TTABLE);
    cm_merkle_tree_proof *proof = new cm_merkle_tree_proof{};
    proof->log2_target_size = check_uint_field(L, tabidx, "log2_target_size");
    proof->log2_root_size = check_uint_field(L, tabidx, "log2_root_size");
    proof->target_address = check_uint_field(L, tabidx, "target_address");
    lua_getfield(L, tabidx, "target_hash");
    clua_check_cm_hash(L, -1, &proof->target_hash);
    lua_pop(L, 1);
    lua_getfield(L, tabidx, "root_hash");
    clua_check_cm_hash(L, -1, &proof->root_hash);
    lua_pop(L, 1);
    lua_getfield(L, tabidx, "sibling_hashes");
    check_sibling_cm_hashes(L, -1, proof->log2_target_size, proof);
    lua_pop(L, 1);
    return proof;
}

/// \brief Returns an access_data field indexed by string in a table.
/// \param L Lua state.
/// \param tabidx Table stack index.
/// \param field Field index.
/// \param log2_size Expected log2 of size of data
/// \param opt Whether filed is optional
/// \returns Field value. Throws error if field is not optional but is missing.
static access_data aux_access_data_field(lua_State *L, int tabidx,
    const char *field, uint64_t log2_size, bool opt) {
    access_data a;
    tabidx = lua_absindex(L, tabidx);
    lua_getfield(L, tabidx, field);
    if (lua_isstring(L, -1)) {
        size_t len = 0;
        const char *s = lua_tolstring(L, -1, &len);
        uint64_t expected_len = UINT64_C(1) << log2_size;
        if (len != expected_len) {
            luaL_error(L, "invalid %s (expected string with 2^%d bytes)", field,
                static_cast<int>(log2_size));
        }
        a.insert(a.end(), s, s+len);
    } else if (!opt || !lua_isnil(L, -1)) {
        luaL_error(L, "invalid %s (expected string)", field);
    }
    lua_pop(L, 1);
    return a;
}

static access_data check_access_data_field(lua_State *L, int tabidx,
    const char *field, uint64_t log2_size) {
    return aux_access_data_field(L, tabidx, field, log2_size, false);
}

static access_data opt_access_data_field(lua_State *L, int tabidx,
    const char *field, uint64_t log2_size) {
    return aux_access_data_field(L, tabidx, field, log2_size, true);
}


/// \brief Returns an access data field indexed by string in a table
/// \param L Lua state
/// \param tabidx Table stack index
/// \param field Field index
/// \param log2_size Expected log2 of size of data
/// \param opt Whether filed is optional
/// \param data_size Receives size of the returned data field
/// \returns Allocated field value. Throws error if field is not optional but is missing.
/// If field is optional but missing returns nullptr
static uint8_t *aux_cm_access_data_field(lua_State *L, int tabidx,
    const char *field, uint64_t log2_size, bool opt, size_t *data_size) {
    uint8_t *a = nullptr;
    *data_size = 0;
    tabidx = lua_absindex(L, tabidx);
    lua_getfield(L, tabidx, field);
    if (lua_isstring(L, -1)) {
        size_t len = 0;
        const char *s = lua_tolstring(L, -1, &len);
        uint64_t expected_len = UINT64_C(1) << log2_size;
        if (len != expected_len) {
            luaL_error(L, "invalid %s (expected string with 2^%d bytes)", field,
                (int) log2_size);
        }
        a = new uint8_t[len];
        memcpy(a, s, len);
        *data_size = len;
    } else if (!opt || !lua_isnil(L, -1)) {
        luaL_error(L, "invalid %s (expected string)", field);
    }
    lua_pop(L, 1);
    return a;
}

static uint8_t *check_cm_access_data_field(lua_State *L, int tabidx, const char *field,
    uint64_t log2_size, size_t *data_size) {
    return aux_cm_access_data_field(L, tabidx, field, log2_size, false, data_size);
}

static uint8_t *opt_cm_access_data_field(lua_State *L, int tabidx, const char *field,
    uint64_t log2_size, size_t *data_size) {
    return aux_cm_access_data_field(L, tabidx, field, log2_size, true, data_size);
}

/// \brief Loads an access from Lua.
/// \param L Lua state.
/// \param tabidx Word_access stack index.
/// \returns The word_access.
static access check_access(lua_State *L, int tabidx, bool proofs) {
    luaL_checktype(L, tabidx, LUA_TTABLE);
    access a;
    if (proofs) {
        lua_getfield(L, tabidx, "proof");
        a.set_proof(clua_check_proof(L, -1));
        lua_pop(L, 1);
    }
    a.set_type(check_access_type_field(L, tabidx, "type"));
    a.set_address(check_uint_field(L, tabidx, "address"));
    a.set_log2_size(check_uint_field(L, tabidx, "log2_size"));
    if (a.get_log2_size() < machine_merkle_tree::get_log2_word_size() ||
        a.get_log2_size() > machine_merkle_tree::get_log2_root_size()) {
        luaL_error(L, "invalid log2_size (expected integer in {%d..%d})",
            machine_merkle_tree::get_log2_word_size(),
            machine_merkle_tree::get_log2_root_size());
    }
    a.set_read(check_access_data_field(L, tabidx, "read", a.get_log2_size()));
    a.set_written(opt_access_data_field(L, tabidx, "written", a.get_log2_size()));
    return a;
}

/// \brief Loads an cm_access from Lua
/// \param L Lua state
/// \param tabidx Word_access stack index
/// \returns The word access
static cm_access check_cm_access(lua_State *L, int tabidx, bool proofs) {
    luaL_checktype(L, tabidx, LUA_TTABLE);
    cm_access a{};
    if (proofs) {
        lua_getfield(L, tabidx, "proof");
        a.proof = clua_check_cm_merkle_tree_proof(L, -1);
        lua_pop(L, 1);
    } else {
        a.proof = nullptr; // redundant with cm_access initialization but intentionally explicit for clarity
    }

    a.type = check_cm_access_type_field(L, tabidx, "type");
    a.address = check_uint_field(L, tabidx, "address");
    a.log2_size = check_uint_field(L, tabidx, "log2_size");
    if (a.log2_size < CM_TREE_LOG2_WORD_SIZE ||
        a.log2_size > CM_TREE_LOG2_ROOT_SIZE) {
        luaL_error(L, "invalid log2_size (expected integer in {%d..%d})",
                   CM_TREE_LOG2_WORD_SIZE, CM_TREE_LOG2_ROOT_SIZE);
    }

    a.read_data = check_cm_access_data_field(L, tabidx, "read", a.log2_size, &a.read_data_size);
    a.written_data = opt_cm_access_data_field(L, tabidx, "written", a.log2_size, &a.written_data_size);
    return a;
}


access_log clua_check_access_log(lua_State *L, int tabidx) {
    std::vector<access> accesses;
    std::vector<bracket_note> brackets;
    std::vector<std::string> notes;
    luaL_checktype(L, tabidx, LUA_TTABLE);
    check_table_field(L, tabidx, "log_type");
    bool proofs = opt_boolean_field(L, -1, "proofs");
    bool annotations = opt_boolean_field(L, -1, "annotations");
    lua_pop(L, 1);
    check_table_field(L, tabidx, "accesses");
    int len = luaL_len(L, -1);
    for (int i = 1; i <= len; i++) {
        lua_geti(L, -1, i);
        if (!lua_istable(L, -1)) {
            luaL_error(L, "access [%d] not a table", i);
        }
        accesses.emplace_back(check_access(L, -1, proofs));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    if (annotations) {
        check_table_field(L, tabidx, "notes");
        len = luaL_len(L, -1);
        for (int i = 1; i <= len; i++) {
            lua_geti(L, -1, i);
            if (!lua_isstring(L, -1)) {
                luaL_error(L, "note [%d] not a string", i);
            }
            notes.emplace_back(lua_tostring(L, -1));
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
        check_table_field(L, tabidx, "brackets");
        len = luaL_len(L, -1);
        for (int i = 1; i <= len; i++) {
            lua_geti(L, -1, i);
            if (!lua_istable(L, -1)) {
                luaL_error(L, "bracket [%d] not a table", i);
            }
            brackets.emplace_back(check_bracket_note(L, -1));
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
    }
    return {
        std::move(accesses),
        std::move(brackets),
        std::move(notes),
        access_log::type{proofs, annotations}
    };
}

cm_access_log* clua_check_cm_access_log(lua_State *L, int tabidx) {

    luaL_checktype(L, tabidx, LUA_TTABLE);
    check_table_field(L, tabidx, "log_type");
    bool proofs = opt_boolean_field(L, -1, "proofs");
    bool annotations = opt_boolean_field(L, -1, "annotations");
    lua_pop(L, 1);

    cm_access_log* acc_log = new cm_access_log{};

    check_table_field(L, tabidx, "accesses");
    acc_log->accesses_count = luaL_len(L, -1);
    acc_log->accesses = new cm_access[acc_log->accesses_count];
    for (size_t i = 1; i <= acc_log->accesses_count; i++) {
        lua_geti(L, -1, i);
        if (!lua_istable(L, -1)) {
            luaL_error(L, "access [%d] not a table", i);
        }
        acc_log->accesses[i-1] = check_cm_access(L, -1, proofs);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    if (annotations) {
        check_table_field(L, tabidx, "notes");
        acc_log->notes_count = luaL_len(L, -1);
        acc_log->notes = new const char *[acc_log->notes_count];
        for (size_t i = 1; i <= acc_log->notes_count; i++) {
            lua_geti(L, -1, i);
            if (!lua_isstring(L, -1)) {
                luaL_error(L, "note [%d] not a string", i);
            }
            acc_log->notes[i-1] = copy_lua_str(L, -1);
            lua_pop(L, 1);
        }
        lua_pop(L, 1);

        check_table_field(L, tabidx, "brackets");
        acc_log->brackets_count = luaL_len(L, -1);
        acc_log->brackets = new cm_bracket_note[acc_log->brackets_count];
        for (size_t i = 1; i <= acc_log->brackets_count; i++) {
            lua_geti(L, -1, i);
            if (!lua_istable(L, -1)) {
                luaL_error(L, "bracket [%d] not a table", i);
            }
            acc_log->brackets[i-1] = check_cm_bracket_note(L, -1);
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
    }

    return acc_log;
}

/// \brief Return a hash from Lua
/// \param L Lua state.
/// \param idx Index in stack.
/// \returns Hash.
machine_merkle_tree::hash_type clua_check_hash(lua_State *L, int idx) {
    machine_merkle_tree::hash_type hash;
    if (lua_isstring(L, idx)) {
        const char *data = nullptr;
        size_t len = 0;
        data = lua_tolstring(L, idx, &len);
        if (len != hash.max_size()) {
            luaL_error(L, "expected hash");
        }
        memcpy(hash.data(), data, hash.max_size());
    } else {
        luaL_error(L, "expected hash");
    }
    return hash;
}

void clua_check_cm_hash(lua_State *L, int idx, cm_hash *c_hash) {
    if (lua_isstring(L, idx)) {
        size_t len = 0;
        const char *data = lua_tolstring(L, idx, &len);
        if (len != sizeof(cm_hash)) {
            luaL_error(L, "expected hash");
        }
        memcpy(c_hash, data, sizeof(cm_hash));
    } else {
        luaL_error(L, "expected hash");
    }
}


machine::csr clua_check_csr(lua_State *L, int idx) {
    std::string name = luaL_checkstring(L, idx);
    auto got = csr_names().find(name);
    if (got == csr_names().end()) {
        luaL_argerror(L, idx, "unknown csr");
    }
    return got->second;
}

CM_PROC_CSR clua_check_cm_proc_csr(lua_State *L, int idx) {
    const char *name = luaL_checkstring(L, idx);
    auto got = g_cm_proc_csr_name.find(name);
    if (got == g_cm_proc_csr_name.end()) {
        luaL_argerror(L, idx, "unknown csr");
    }
    return got->second;
}

/// \brief Pushes C array of data to the Lua stack
/// \param L Lua state.
/// \param data Pointer to C array of data
/// \param data_size Size of array of data
static void push_raw_data(lua_State *L, const uint8_t* data, size_t data_size) {
    lua_pushlstring(L, reinterpret_cast<const char *>(data), data_size);
}

/// \brief Pushes an access_log::type to the Lua stack
/// \param L Lua state.
/// \param log_type Access_log::type to be pushed.
static void push_log_type(lua_State *L, bool proofs, bool annotations) {
    lua_newtable(L);
    lua_pushboolean(L, annotations);
    lua_setfield(L, -2, "annotations");
    lua_pushboolean(L, proofs);
    lua_setfield(L, -2, "proofs");
}

/// \brief Pushes a cm_access_log_type to the Lua stack
/// \param L Lua state.
/// \param log_type Pointer to cm_access_log_type to be pushed.
static void push_cm_access_log_type(lua_State *L, const cm_access_log_type *log_type) {
    lua_newtable(L);
    lua_pushboolean(L, log_type->annotations);
    lua_setfield(L, -2, "annotations");
    lua_pushboolean(L, log_type->proofs);
    lua_setfield(L, -2, "proofs");
}

/// \brief Converts an access_type to a string.
/// \param type Access type.
/// \returns String with access_type name.
static const char *access_type_name(access_type type) {
    switch (type) {
        case access_type::read:
            return "read";
        case access_type::write:
            return "write";
    }
    return nullptr;
}

/// \brief Converts an CM_ACCESS_TYPE to a string.
/// \param type Access type.
/// \returns String with access type name.
static const char *cm_access_type_name(CM_ACCESS_TYPE type) {
    switch (type) {
        case CM_ACCESS_READ:
            return "read";
        case CM_ACCESS_WRITE:
            return "write";
    }
    return nullptr;
}

/// \brief Converts a bracket_type to a string
/// \param type Bracket type.
/// \returns String with bracket type name.
static const char *bracket_type_name(bracket_type type) {
    switch (type) {
        case bracket_type::begin:
            return "begin";
        case bracket_type::end:
            return "end";
    }
    return nullptr;
}

/// \brief Converts a CM_BRACKET_TYPE to a string
/// \param type Bracket type.
/// \returns String with bracket type name.
static const char *cm_bracket_type_name(CM_BRACKET_TYPE type) {
    switch (type) {
        case CM_BRACKET_BEGIN:
            return "begin";
        case CM_BRACKET_END:
            return "end";
    }
    return nullptr;
}

/// \brief Pushes an access_data to the Lua stack
/// \param L Lua state.
/// \param a Access_data to be pushed.
static void push_access_data(lua_State *L, const access_data &a) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    lua_pushlstring(L, reinterpret_cast<const char *>(a.data()), a.size());
}

/// \brief Pushes an access log to the Lua stack
/// \param L Lua state.
/// \param log Access log to be pushed.
void clua_push_access_log(lua_State *L, const access_log &log) {
    lua_newtable(L); // log
    lua_newtable(L); // log type
    auto log_type = log.get_log_type();
    push_log_type(L, log_type.has_proofs(), log_type.has_annotations());
    lua_setfield(L, -2, "log_type"); // log
    // Add all accesses
    lua_newtable(L); // log accesses
    int i = 1; // convert from 0- to 1-based index
    for (const auto &a: log.get_accesses()) {
        lua_newtable(L); // log accesses wordaccess
        lua_pushstring(L, access_type_name(a.get_type()));
        lua_setfield(L, -2, "type");
        lua_pushinteger(L, a.get_address());
        lua_setfield(L, -2, "address");
        lua_pushinteger(L, a.get_log2_size());
        lua_setfield(L, -2, "log2_size");
        push_access_data(L, a.get_read());
        lua_setfield(L, -2, "read");
        if (a.get_type() == access_type::write) {
            push_access_data(L, a.get_written());
            lua_setfield(L, -2, "written");
        }
        if (log_type.has_proofs() && a.get_proof().has_value()) {
            clua_push_proof(L, a.get_proof().value());
            lua_setfield(L, -2, "proof");
        }
        lua_rawseti(L, -2, i);
        ++i;
    }
    lua_setfield(L, -2, "accesses"); // log
    // Add all brackets
    if (log_type.has_annotations()) {
        lua_newtable(L); // log brackets
        i = 1; // convert from 0- to 1-based index
        for (const auto &b: log.get_brackets()) {
            lua_newtable(L); // log brackets bracket
            lua_pushstring(L, bracket_type_name(b.type));
            lua_setfield(L, -2, "type");
            lua_pushinteger(L, b.where+1); // convert from 0- to 1-based index
            lua_setfield(L, -2, "where");
            lua_pushlstring(L, b.text.data(), b.text.size());
            lua_setfield(L, -2, "text");
            lua_rawseti(L, -2, i);
            ++i;
        }
        lua_setfield(L, -2, "brackets"); // log

        lua_newtable(L); // log notes
        i = 1; // convert from 0- to 1-based index
        for (const auto &n: log.get_notes()) {
            lua_pushlstring(L, n.data(), n.size());
            lua_rawseti(L, -2, i);
            i++;
        }
        lua_setfield(L, -2, "notes"); // log
    }
}

void clua_push_cm_access_log(lua_State *L, const cm_access_log *log) {
    lua_newtable(L); // log
    lua_newtable(L); // log type
    push_cm_access_log_type(L, &log->log_type);
    lua_setfield(L, -2, "log_type"); // log

    // Add all accesses
    lua_newtable(L); // log accesses
    for (size_t i = 0; i < log->accesses_count; ++i) {
        const cm_access *a = &log->accesses[i];
        lua_newtable(L); // log accesses wordaccess
        lua_pushstring(L, cm_access_type_name(a->type));
        lua_setfield(L, -2, "type");
        lua_pushinteger(L, a->address);
        lua_setfield(L, -2, "address");
        lua_pushinteger(L, a->log2_size);
        lua_setfield(L, -2, "log2_size");
        push_raw_data(L, a->read_data, a->read_data_size);
        lua_setfield(L, -2, "read");
        if (a->type == CM_ACCESS_WRITE) {
            push_raw_data(L, a->written_data, a->written_data_size);
            lua_setfield(L, -2, "written");
        }
        if (log->log_type.proofs && a->proof != nullptr) {
            clua_push_cm_proof(L, a->proof);
            lua_setfield(L, -2, "proof");
        }
        lua_rawseti(L, -2, i + 1);
    }
    lua_setfield(L, -2, "accesses"); // log
    // Add all brackets
    if (log->log_type.annotations) {
        lua_newtable(L); // log brackets
        for (size_t i = 0; i < log->brackets_count; ++i) {
            const cm_bracket_note *b = &log->brackets[i];
            lua_newtable(L); // log brackets bracket
            lua_pushstring(L, cm_bracket_type_name(b->type));
            lua_setfield(L, -2, "type");
            lua_pushinteger(L, b->where + 1); // convert from 0- to 1-based index
            lua_setfield(L, -2, "where");
            lua_pushstring(L, b->text);
            lua_setfield(L, -2, "text");
            lua_rawseti(L, -2, i + 1);
        }
        lua_setfield(L, -2, "brackets"); // log

        lua_newtable(L); // log notes
        for (size_t i = 0; i < log->notes_count; ++i) {
            const char *note = log->notes[i];
            lua_pushstring(L, note);
            lua_rawseti(L, -2, i + 1);
        }
        lua_setfield(L, -2, "notes"); // log
    }
}

void clua_push_hash(lua_State *L, const machine_merkle_tree::hash_type &hash) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    lua_pushlstring(L, reinterpret_cast<const char *>(hash.data()),
        hash.size());
}

void clua_push_cm_hash(lua_State *L, const cm_hash *hash) {
    lua_pushlstring(L, reinterpret_cast<const char *>(hash),
        CM_MACHINE_HASH_BYTE_SIZE);
}

void clua_push_semantic_version(lua_State *L, const semantic_version &v) {
    lua_newtable(L); // version
    lua_pushinteger(L, v.major); lua_setfield(L, -2, "major"); // version
    lua_pushinteger(L, v.minor); lua_setfield(L, -2, "minor"); // version
    lua_pushinteger(L, v.patch); lua_setfield(L, -2, "patch"); // version
    lua_pushlstring(L, v.pre_release.data(), v.pre_release.size()); lua_setfield(L, -2, "pre_release"); // version
    lua_pushlstring(L, v.build.data(), v.build.size()); lua_setfield(L, -2, "build"); // version
}

void clua_push_proof(lua_State *L, const machine_merkle_tree::proof_type &proof) {
    lua_newtable(L); // proof
    lua_newtable(L); // proof siblings
    for (int log2_size = proof.get_log2_target_size();
        log2_size < proof.get_log2_root_size(); ++log2_size) {
        clua_push_hash(L, proof.get_sibling_hash(log2_size));
        lua_rawseti(L, -2, proof.get_log2_root_size()-log2_size);
    }
    lua_setfield(L, -2, "sibling_hashes"); // proof
    lua_pushinteger(L, proof.get_target_address()); lua_setfield(L, -2, "target_address"); // proof
    lua_pushinteger(L, proof.get_log2_target_size()); lua_setfield(L, -2, "log2_target_size"); // proof
    lua_pushinteger(L, proof.get_log2_root_size()); lua_setfield(L, -2, "log2_root_size"); // proof
    clua_push_hash(L, proof.get_root_hash()); lua_setfield(L, -2, "root_hash"); // proof
    clua_push_hash(L, proof.get_target_hash()); lua_setfield(L, -2, "target_hash"); // proof
}

void clua_push_cm_proof(lua_State *L, const cm_merkle_tree_proof *proof) {
    lua_newtable(L); // proof
    lua_newtable(L); // proof siblings
    for (size_t log2_size = proof->log2_target_size;
         log2_size < proof->log2_root_size; ++log2_size) {
        clua_push_cm_hash(L, &proof->sibling_hashes[proof->log2_root_size - 1 - log2_size]);
        lua_rawseti(L, -2, proof->log2_root_size-log2_size);
    }
    lua_setfield(L, -2, "sibling_hashes"); // proof
    lua_pushinteger(L, proof->target_address); lua_setfield(L, -2, "target_address"); // proof
    lua_pushinteger(L, proof->log2_target_size); lua_setfield(L, -2, "log2_target_size"); // proof
    lua_pushinteger(L, proof->log2_root_size); lua_setfield(L, -2, "log2_root_size"); // proof
    clua_push_cm_hash(L, &proof->root_hash); lua_setfield(L, -2, "root_hash"); // proof
    clua_push_cm_hash(L, &proof->target_hash); lua_setfield(L, -2, "target_hash"); // proof
}

access_log::type clua_check_log_type(lua_State *L, int tabidx) {
    luaL_checktype(L, tabidx, LUA_TTABLE);
    return access_log::type{
        opt_boolean_field(L, -1, "proofs"),
        opt_boolean_field(L, -1, "annotations")
    };
}

cm_access_log_type clua_check_cm_log_type(lua_State *L, int tabidx) {
    luaL_checktype(L, tabidx, LUA_TTABLE);

    return cm_access_log_type {
        opt_boolean_field(L, tabidx, "proofs"),
        opt_boolean_field(L, tabidx, "annotations")
    };
}

#define PUSH_PROCESSOR_CONFIG_CSR(regname) \
    do {                                   \
        lua_pushinteger(L, p.regname);     \
        lua_setfield(L, -2, #regname);     \
    } while (0)


/// \brief Pushes a processor_config to the Lua stack
/// \param L Lua state.
/// \param p Processor_config to be pushed.
static void push_processor_config(lua_State *L, const processor_config &p) {
    lua_newtable(L); // p
    lua_newtable(L); // p x
    for (int i = 1; i <= (X_REG_COUNT-1); i++) {
        lua_pushinteger(L, p.x[i]);
        lua_rawseti(L, -2, i);
    }
    lua_setfield(L, -2, "x");
    PUSH_PROCESSOR_CONFIG_CSR(pc);
    PUSH_PROCESSOR_CONFIG_CSR(mvendorid);
    PUSH_PROCESSOR_CONFIG_CSR(marchid);
    PUSH_PROCESSOR_CONFIG_CSR(mimpid);
    PUSH_PROCESSOR_CONFIG_CSR(mcycle);
    PUSH_PROCESSOR_CONFIG_CSR(minstret);
    PUSH_PROCESSOR_CONFIG_CSR(mstatus);
    PUSH_PROCESSOR_CONFIG_CSR(mtvec);
    PUSH_PROCESSOR_CONFIG_CSR(mscratch);
    PUSH_PROCESSOR_CONFIG_CSR(mepc);
    PUSH_PROCESSOR_CONFIG_CSR(mcause);
    PUSH_PROCESSOR_CONFIG_CSR(mtval);
    PUSH_PROCESSOR_CONFIG_CSR(misa);
    PUSH_PROCESSOR_CONFIG_CSR(mie);
    PUSH_PROCESSOR_CONFIG_CSR(mip);
    PUSH_PROCESSOR_CONFIG_CSR(medeleg);
    PUSH_PROCESSOR_CONFIG_CSR(mideleg);
    PUSH_PROCESSOR_CONFIG_CSR(mcounteren);
    PUSH_PROCESSOR_CONFIG_CSR(stvec);
    PUSH_PROCESSOR_CONFIG_CSR(sscratch);
    PUSH_PROCESSOR_CONFIG_CSR(sepc);
    PUSH_PROCESSOR_CONFIG_CSR(scause);
    PUSH_PROCESSOR_CONFIG_CSR(stval);
    PUSH_PROCESSOR_CONFIG_CSR(satp);
    PUSH_PROCESSOR_CONFIG_CSR(scounteren);
    PUSH_PROCESSOR_CONFIG_CSR(ilrsc);
    PUSH_PROCESSOR_CONFIG_CSR(iflags);
}


#define PUSH_CM_PROCESSOR_CONFIG_CSR(regname) \
    do {                                      \
        lua_pushinteger(L, p->regname);       \
        lua_setfield(L, -2, #regname);        \
    } while (0)

/// \brief Pushes a cm_processor_config to the Lua stack
/// \param L Lua state.
/// \param p Processor_config to be pushed.
static void push_cm_processor_config(lua_State *L, const cm_processor_config *p) {
    lua_newtable(L); // p
    lua_newtable(L); // p x
    for (int i = 1; i <= (X_REG_COUNT-1); i++) {
        lua_pushinteger(L, p->x[i-1]);
        lua_rawseti(L, -2, i);
    }
    lua_setfield(L, -2, "x");
    PUSH_CM_PROCESSOR_CONFIG_CSR(pc);
    PUSH_CM_PROCESSOR_CONFIG_CSR(mvendorid);
    PUSH_CM_PROCESSOR_CONFIG_CSR(marchid);
    PUSH_CM_PROCESSOR_CONFIG_CSR(mimpid);
    PUSH_CM_PROCESSOR_CONFIG_CSR(mcycle);
    PUSH_CM_PROCESSOR_CONFIG_CSR(minstret);
    PUSH_CM_PROCESSOR_CONFIG_CSR(mstatus);
    PUSH_CM_PROCESSOR_CONFIG_CSR(mtvec);
    PUSH_CM_PROCESSOR_CONFIG_CSR(mscratch);
    PUSH_CM_PROCESSOR_CONFIG_CSR(mepc);
    PUSH_CM_PROCESSOR_CONFIG_CSR(mcause);
    PUSH_CM_PROCESSOR_CONFIG_CSR(mtval);
    PUSH_CM_PROCESSOR_CONFIG_CSR(misa);
    PUSH_CM_PROCESSOR_CONFIG_CSR(mie);
    PUSH_CM_PROCESSOR_CONFIG_CSR(mip);
    PUSH_CM_PROCESSOR_CONFIG_CSR(medeleg);
    PUSH_CM_PROCESSOR_CONFIG_CSR(mideleg);
    PUSH_CM_PROCESSOR_CONFIG_CSR(mcounteren);
    PUSH_CM_PROCESSOR_CONFIG_CSR(stvec);
    PUSH_CM_PROCESSOR_CONFIG_CSR(sscratch);
    PUSH_CM_PROCESSOR_CONFIG_CSR(sepc);
    PUSH_CM_PROCESSOR_CONFIG_CSR(scause);
    PUSH_CM_PROCESSOR_CONFIG_CSR(stval);
    PUSH_CM_PROCESSOR_CONFIG_CSR(satp);
    PUSH_CM_PROCESSOR_CONFIG_CSR(scounteren);
    PUSH_CM_PROCESSOR_CONFIG_CSR(ilrsc);
    PUSH_CM_PROCESSOR_CONFIG_CSR(iflags);
}

/// \brief Pushes a ram_config to the Lua stack
/// \param L Lua state.
/// \param r Ram_config to be pushed.
static void push_ram_config(lua_State *L, const ram_config &r) {
    lua_newtable(L);
    lua_pushinteger(L, r.length); lua_setfield(L, -2, "length");
    if (!r.image_filename.empty()) {
        lua_pushstring(L, r.image_filename.c_str());
        lua_setfield(L, -2, "image_filename");
    }
}

/// \brief Pushes a cm_ram_config to the Lua stack
/// \param L Lua state.
/// \param r Ram configuration to be pushed.
static void push_cm_ram_config(lua_State *L, const cm_ram_config *r) {
    lua_newtable(L);
    lua_pushinteger(L, r->length); lua_setfield(L, -2, "length");
    if (r->image_filename != nullptr) {
        lua_pushstring(L, r->image_filename);
        lua_setfield(L, -2, "image_filename");
    }
}

/// \brief Pushes a rom_config to the Lua stack
/// \param L Lua state.
/// \param r Ram_config to be pushed.
static void push_rom_config(lua_State *L, const rom_config &r) {
    lua_newtable(L);
    if (!r.bootargs.empty()) {
        lua_pushstring(L, r.bootargs.c_str());
        lua_setfield(L, -2, "bootargs");
    }
    if (!r.image_filename.empty()) {
        lua_pushstring(L, r.image_filename.c_str());
        lua_setfield(L, -2, "image_filename");
    }
}

/// \brief Pushes a cm_rom_config to the Lua stack
/// \param L Lua state.
/// \param r Ram configuration to be pushed.
static void push_cm_rom_config(lua_State *L, const cm_rom_config *r) {
    lua_newtable(L);
    if (r->bootargs != nullptr) {
        lua_pushstring(L, r->bootargs);
        lua_setfield(L, -2, "bootargs");
    }
    if (r->image_filename) {
        lua_pushstring(L, r->image_filename);
        lua_setfield(L, -2, "image_filename");
    }
}

/// \brief Pushes an htif_config to the Lua stack
/// \param L Lua state.
/// \param h Htif_config to be pushed.
static void push_htif_config(lua_State *L, const htif_config &h) {
    lua_newtable(L);
    lua_pushboolean(L, h.console_getchar); lua_setfield(L, -2, "console_getchar");
    lua_pushboolean(L, h.yield_progress); lua_setfield(L, -2, "yield_progress");
    lua_pushboolean(L, h.yield_rollup); lua_setfield(L, -2, "yield_rollup");
    lua_pushinteger(L, h.fromhost); lua_setfield(L, -2, "fromhost");
    lua_pushinteger(L, h.tohost); lua_setfield(L, -2, "tohost");
}

/// \brief Pushes an cm_htif_config to the Lua stack
/// \param L Lua state.
/// \param h Htif configuration to be pushed.
static void push_cm_htif_config(lua_State *L, const cm_htif_config *h) {
    lua_newtable(L);
    lua_pushboolean(L, h->console_getchar); lua_setfield(L, -2, "console_getchar");
    lua_pushboolean(L, h->yield_progress); lua_setfield(L, -2, "yield_progress");
    lua_pushboolean(L, h->yield_rollup); lua_setfield(L, -2, "yield_rollup");
    lua_pushinteger(L, h->fromhost); lua_setfield(L, -2, "fromhost");
    lua_pushinteger(L, h->tohost); lua_setfield(L, -2, "tohost");
}

/// \brief Pushes an clint_config to the Lua stack
/// \param L Lua state.
/// \param c Clint_config to be pushed.
static void push_clint_config(lua_State *L, const clint_config &c) {
    lua_newtable(L);
    lua_pushinteger(L, c.mtimecmp); lua_setfield(L, -2, "mtimecmp");
}

/// \brief Pushes an cm_clint_config to the Lua stack
/// \param L Lua state.
/// \param c Clint configuration to be pushed.
static void push_cm_clint_config(lua_State *L, const cm_clint_config *c) {
    lua_newtable(L);
    lua_pushinteger(L, c->mtimecmp); lua_setfield(L, -2, "mtimecmp");
}

/// \brief Pushes an dhd_config to the Lua stack
/// \param L Lua state.
/// \param c Dhd_config to be pushed.
static void push_dhd_config(lua_State *L, const dhd_config &d) {
    lua_newtable(L);
    lua_pushinteger(L, d.tstart); lua_setfield(L, -2, "tstart");
    lua_pushinteger(L, d.tlength); lua_setfield(L, -2, "tlength");
    if (!d.image_filename.empty()) {
        lua_pushstring(L, d.image_filename.c_str());
        lua_setfield(L, -2, "image_filename");
    }
    lua_pushinteger(L, d.dlength); lua_setfield(L, -2, "dlength");
    lua_pushinteger(L, d.hlength); lua_setfield(L, -2, "hlength");
    lua_newtable(L);
    for (int i = 1; i <= DHD_H_REG_COUNT; i++) {
        lua_pushinteger(L, d.h[i-1]); lua_rawseti(L, -2, i);
    }
    lua_setfield(L, -2, "h");
}

/// \brief Pushes an cm_dhd_config to the Lua stack
/// \param L Lua state.
/// \param c Dhd configuration to be pushed.
static void push_cm_dhd_config(lua_State *L, const cm_dhd_config *d) {
    lua_newtable(L);
    lua_pushinteger(L, d->tstart); lua_setfield(L, -2, "tstart");
    lua_pushinteger(L, d->tlength); lua_setfield(L, -2, "tlength");
    if (d->image_filename != nullptr) {
        lua_pushstring(L, d->image_filename);
        lua_setfield(L, -2, "image_filename");
    }
    lua_pushinteger(L, d->dlength); lua_setfield(L, -2, "dlength");
    lua_pushinteger(L, d->hlength); lua_setfield(L, -2, "hlength");
    lua_newtable(L);
    for (int i = 1; i <= CM_MACHINE_DHD_H_REG_COUNT; i++) {
        lua_pushinteger(L, d->h[i-1]); lua_rawseti(L, -2, i);
    }
    lua_setfield(L, -2, "h");
}

/// \brief Pushes flash_drive_configs to the Lua stack
/// \param L Lua state.
/// \param flash_drive Flash_drive_configs to be pushed.
static void push_flash_drive_configs(lua_State *L,
    const flash_drive_configs &flash_drive) {
    lua_newtable(L);
    int i = 1;
    for (const auto &f: flash_drive) {
        lua_newtable(L);
        lua_pushinteger(L, f.start); lua_setfield(L, -2, "start");
        lua_pushinteger(L, f.length); lua_setfield(L, -2, "length");
        if (!f.image_filename.empty()) {
            lua_pushstring(L, f.image_filename.c_str());
            lua_setfield(L, -2, "image_filename");
        }
        lua_pushboolean(L, f.shared); lua_setfield(L, -2, "shared");
        lua_rawseti(L, -2, i);
        i++;
    }
}

/// \brief Pushes cm_flash_drive_config to the Lua stack
/// \param L Lua state.
/// \param flash_drive Flash drive configurations to be pushed.
static void push_cm_flash_drive_configs(lua_State *L, const cm_flash_drive_config flash_drive[],
                                     size_t flash_drive_count) {
    lua_newtable(L);
    for (size_t j = 0; j < flash_drive_count; ++j) {
        lua_newtable(L);
        const cm_flash_drive_config *f = &flash_drive[j];
        lua_pushinteger(L, f->start);
        lua_setfield(L, -2, "start");
        lua_pushinteger(L, f->length);
        lua_setfield(L, -2, "length");
        if (f->image_filename != nullptr) {
            lua_pushstring(L, f->image_filename);
            lua_setfield(L, -2, "image_filename");
        }
        lua_pushboolean(L, f->shared);
        lua_setfield(L, -2, "shared");
        lua_rawseti(L, -2, j+1);
    }
}

void clua_push_machine_config(lua_State *L, const machine_config &c) {
    lua_newtable(L); // config
    push_processor_config(L, c.processor); // config processor
    lua_setfield(L, -2, "processor"); // config
    push_htif_config(L, c.htif); // config htif
    lua_setfield(L, -2, "htif"); // config
    push_clint_config(L, c.clint); // config clint
    lua_setfield(L, -2, "clint"); // config
    push_flash_drive_configs(L, c.flash_drive); // config flash_drive
    lua_setfield(L, -2, "flash_drive"); // config
    push_ram_config(L, c.ram); // config ram
    lua_setfield(L, -2, "ram"); // config
    push_rom_config(L, c.rom); // config rom
    lua_setfield(L, -2, "rom"); // config
    push_dhd_config(L, c.dhd); // config dhd
    lua_setfield(L, -2, "dhd"); // config
}

void clua_push_cm_machine_config(lua_State *L, const cm_machine_config *c) {
    lua_newtable(L); // config
    push_cm_processor_config(L, &c->processor); // config processor
    lua_setfield(L, -2, "processor"); // config
    push_cm_htif_config(L, &c->htif); // config htif
    lua_setfield(L, -2, "htif"); // config
    push_cm_clint_config(L, &c->clint); // config clint
    lua_setfield(L, -2, "clint"); // config
    push_cm_flash_drive_configs(L, c->flash_drive, c->flash_drive_count); // config flash_drive
    lua_setfield(L, -2, "flash_drive"); // config
    push_cm_ram_config(L, &c->ram); // config ram
    lua_setfield(L, -2, "ram"); // config
    push_cm_rom_config(L, &c->rom); // config rom
    lua_setfield(L, -2, "rom"); // config
    push_cm_dhd_config(L, &c->dhd); // config dhd
    lua_setfield(L, -2, "dhd"); // config
}


/// \brief Pushes an dhd_runtime_config to the Lua stack
/// \param L Lua state.
/// \param c Dhd_runtime_config to be pushed.
static void push_dhd_runtime_config(lua_State *L, const dhd_runtime_config &d) {
    lua_newtable(L);
    if (!d.source_address.empty()) {
        lua_pushlstring(L, d.source_address.data(), d.source_address.size());
        lua_setfield(L, -2, "source_address");
    }
}

/// \brief Pushes an cm_dhd_runtime_config to the Lua stack
/// \param L Lua state.
/// \param c C api dhd runtime config to be pushed.
static void push_cm_dhd_runtime_config(lua_State *L, const cm_dhd_runtime_config *d) {
    lua_newtable(L);
    if (d->source_address != nullptr) {
        lua_pushstring(L, d->source_address);
        lua_setfield(L, -2, "source_address");
    }
}

/// \brief Pushes an concurrency_config to the Lua stack
/// \param L Lua state.
/// \param c concurrency_config to be pushed.
static void push_concurrency_runtime_config(lua_State *L, const concurrency_config &c) {
    lua_newtable(L);
    lua_pushinteger(L, c.update_merkle_tree);
    lua_setfield(L, -2, "update_merkle_tree");
}

/// \brief Pushes an cm_concurrency_config to the Lua stack
/// \param L Lua state.
/// \param c C api concurrency config to be pushed.
static void push_cm_concurrency_runtime_config(lua_State *L, const cm_concurrency_config *c) {
    lua_newtable(L);
    lua_pushinteger(L, c->update_merkle_tree);
    lua_setfield(L, -2, "update_merkle_tree");
}

void clua_push_machine_runtime_config(lua_State *L,
    const machine_runtime_config &r) {
    lua_newtable(L); // config
    push_dhd_runtime_config(L, r.dhd); // config dhd
    lua_setfield(L, -2, "dhd"); // config
    push_concurrency_runtime_config(L, r.concurrency); // config concurrency
    lua_setfield(L, -2, "concurrency"); // config
}

void clua_push_cm_machine_runtime_config(lua_State *L,
    const cm_machine_runtime_config *r) {
    lua_newtable(L); // config
    push_cm_dhd_runtime_config(L, &r->dhd); // config dhd
    lua_setfield(L, -2, "dhd"); // config
    push_cm_concurrency_runtime_config(L, &r->concurrency); // config concurrency
    lua_setfield(L, -2, "concurrency"); // config
}

/// \brief Loads RAM config from Lua to machine_config.
/// \param L Lua state.
/// \param tabidx Config stack index.
/// \param r RAM config structure to receive results.
static void check_ram_config(lua_State *L, int tabidx, ram_config &r) {
    check_table_field(L, tabidx, "ram");
    r.length = check_uint_field(L, -1, "length");
    r.image_filename = opt_string_field(L, -1, "image_filename");
    lua_pop(L, 1);
}

/// \brief Loads RAM config from Lua to cm_machine_config.
/// \param L Lua state.
/// \param tabidx Config stack index.
/// \param r C api RAM config structure to receive results.
static void check_cm_ram_config(lua_State *L, int tabidx, cm_ram_config *r) {
    check_table_field(L, tabidx, "ram");
    r->length = check_uint_field(L, -1, "length");
    r->image_filename = opt_string_field_c(L, -1, "image_filename");
    lua_pop(L, 1);
}

/// \brief Loads ROM config from Lua to rom_config
/// \param L Lua state.
/// \param tabidx Config stack index.
/// \param r ROM config structure to receive results.
static void check_rom_config(lua_State *L, int tabidx, rom_config &r) {
    if (!opt_table_field(L, tabidx, "rom")) {
        return;
    }
    r.image_filename = opt_string_field(L, -1, "image_filename");
    r.bootargs = opt_string_field(L, -1, "bootargs");
    lua_pop(L, 1);
}

/// \brief Loads ROM config from Lua to cm_rom_config
/// \param L Lua state
/// \param tabidx Config stack index
/// \param r C api ROM config structure to receive results
static void check_cm_rom_config(lua_State *L, int tabidx, cm_rom_config *r) {
    if (!opt_table_field(L, tabidx, "rom")) {
        return;
    }
    r->image_filename = opt_string_field_c(L, -1, "image_filename");
    r->bootargs = opt_string_field_c(L, -1, "bootargs");
    lua_pop(L, 1);
}

flash_drive_config clua_check_flash_drive_config(lua_State *L, int tabidx) {
    if (!lua_istable(L, tabidx)) {
        luaL_error(L, "flash drive not a table");
    }
    flash_drive_config f;
    f.shared = opt_boolean_field(L, tabidx, "shared");
    f.image_filename = opt_string_field(L, tabidx, "image_filename");
    f.start = check_uint_field(L, tabidx, "start");
    f.length = check_uint_field(L, tabidx, "length");
    return f;
}

cm_flash_drive_config clua_check_cm_flash_drive_config(lua_State *L, int tabidx) {
    if (!lua_istable(L, tabidx)) {
        luaL_error(L, "flash drive not a table");
    }
    cm_flash_drive_config f{};
    f.shared = opt_boolean_field(L, tabidx, "shared");
    f.image_filename = opt_string_field_c(L, tabidx, "image_filename");
    f.start = check_uint_field(L, tabidx, "start");
    f.length = check_uint_field(L, tabidx, "length");
    return f;
}

/// \brief Loads flash drive configs from a Lua machine config.
/// \param L Lua state.
/// \param tabidx Machine config stack index.
/// \param f Flash_configs structure to receive results.
static void check_flash_drive_configs(lua_State *L, int tabidx,
    flash_drive_configs &fs) {
    if (!opt_table_field(L, tabidx, "flash_drive")) {
        return;
    }
    int len = luaL_len(L, -1);
    if (len > static_cast<int>(fs.capacity())) { // NOLINT(readability-static-accessed-through-instance)
        luaL_error(L, "too many flash drives");
    }
    for (int i = 1; i <= len; i++) {
        lua_geti(L, -1, i);
        fs.push_back(clua_check_flash_drive_config(L, -1));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

/// \brief Loads a C api flash drive configs from a Lua machine config
/// \param L Lua state
/// \param tabidx Machine config stack index
/// \param fs Receives allocate array of flash drive configs
/// \returns Number of flash drives acquired
static size_t check_cm_flash_drive_configs(lua_State *L, int tabidx,
    cm_flash_drive_config **fs) {
    if (!opt_table_field(L, tabidx, "flash_drive")) {
        return 0;
    }
    auto count = luaL_len(L, -1);
    *fs = new cm_flash_drive_config[count];

    for (int i = 1; i <= count; i++) {
        lua_geti(L, -1, i);
        (*fs)[i-1] = clua_check_cm_flash_drive_config(L, -1);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    return count;
}

/// \brief Loads processor config from a Lua to machine config
/// \param L Lua state.
/// \param tabidx Config stack index.
/// \param p Processor config structure to receive results.
static void check_processor_config(lua_State *L, int tabidx,
    processor_config &p) {
    if (!opt_table_field(L, tabidx, "processor")) {
        return;
    }
    lua_getfield(L, -1, "x");
    if (lua_istable(L, -1)) {
        for (int i = 1; i < X_REG_COUNT; i++) {
            p.x[i] = opt_uint_field(L, -1, i, p.x[i]);
        }
    } else if (!lua_isnil(L, -1)) {
        luaL_error(L, "invalid processor.x (expected table)");
    }
    lua_pop(L, 1);
    p.pc = opt_uint_field(L, -1, "pc", p.pc);
    p.mvendorid = opt_uint_field(L, -1, "mvendorid", p.mvendorid);
    p.marchid = opt_uint_field(L, -1, "marchid", p.marchid);
    p.mimpid = opt_uint_field(L, -1, "mimpid", p.mimpid);
    p.mcycle = opt_uint_field(L, -1, "mcycle", p.mcycle);
    p.minstret = opt_uint_field(L, -1, "minstret", p.minstret);
    p.mstatus = opt_uint_field(L, -1, "mstatus", p.mstatus);
    p.mtvec = opt_uint_field(L, -1, "mtvec", p.mtvec);
    p.mscratch = opt_uint_field(L, -1, "mscratch", p.mscratch);
    p.mepc = opt_uint_field(L, -1, "mepc", p.mepc);
    p.mcause = opt_uint_field(L, -1, "mcause", p.mcause);
    p.mtval = opt_uint_field(L, -1, "mtval", p.mtval);
    p.misa = opt_uint_field(L, -1, "misa", p.misa);
    p.mie = opt_uint_field(L, -1, "mie", p.mie);
    p.mip = opt_uint_field(L, -1, "mip", p.mip);
    p.medeleg = opt_uint_field(L, -1, "medeleg", p.medeleg);
    p.mideleg = opt_uint_field(L, -1, "mideleg", p.mideleg);
    p.mcounteren = opt_uint_field(L, -1, "mcounteren", p.mcounteren);
    p.stvec = opt_uint_field(L, -1, "stvec", p.stvec);
    p.sscratch = opt_uint_field(L, -1, "sscratch", p.sscratch);
    p.sepc = opt_uint_field(L, -1, "sepc", p.sepc);
    p.scause = opt_uint_field(L, -1, "scause", p.scause);
    p.stval = opt_uint_field(L, -1, "stval", p.stval);
    p.satp = opt_uint_field(L, -1, "satp", p.satp);
    p.scounteren = opt_uint_field(L, -1, "scounteren", p.scounteren);
    p.ilrsc = opt_uint_field(L, -1, "ilrsc", p.ilrsc);
    p.iflags = opt_uint_field(L, -1, "iflags", p.iflags);
    lua_pop(L, 1);
}


/// \brief Loads processor config from a Lua to C api machine config
/// \param L Lua state
/// \param tabidx Config stack index
/// \param p Pointer to C api processor config structure to receive results
/// \param def Default configuration
static void check_cm_processor_config(lua_State *L, int tabidx,
    cm_processor_config *p, const cm_processor_config *def) {
    if (!opt_table_field(L, tabidx, "processor")) {
        *p = *def;
        return;
    }
    lua_getfield(L, -1, "x");
    if (lua_istable(L, -1)) {
        for (int i = 1; i < X_REG_COUNT; i++) {
            p->x[i] = opt_uint_field(L, -1, i, def->x[i]);
        }
    } else if (!lua_isnil(L, -1)) {
        luaL_error(L, "invalid processor.x (expected table)");
    }
    lua_pop(L, 1);
    p->pc = opt_uint_field(L, -1, "pc", def->pc);
    p->mvendorid = opt_uint_field(L, -1, "mvendorid", def->mvendorid);
    p->marchid = opt_uint_field(L, -1, "marchid", def->marchid);
    p->mimpid = opt_uint_field(L, -1, "mimpid", def->mimpid);
    p->mcycle = opt_uint_field(L, -1, "mcycle", def->mcycle);
    p->minstret = opt_uint_field(L, -1, "minstret", def->minstret);
    p->mstatus = opt_uint_field(L, -1, "mstatus", def->mstatus);
    p->mtvec = opt_uint_field(L, -1, "mtvec", def->mtvec);
    p->mscratch = opt_uint_field(L, -1, "mscratch", def->mscratch);
    p->mepc = opt_uint_field(L, -1, "mepc", def->mepc);
    p->mcause = opt_uint_field(L, -1, "mcause", def->mcause);
    p->mtval = opt_uint_field(L, -1, "mtval", def->mtval);
    p->misa = opt_uint_field(L, -1, "misa", def->misa);
    p->mie = opt_uint_field(L, -1, "mie", def->mie);
    p->mip = opt_uint_field(L, -1, "mip", def->mip);
    p->medeleg = opt_uint_field(L, -1, "medeleg", def->medeleg);
    p->mideleg = opt_uint_field(L, -1, "mideleg", def->mideleg);
    p->mcounteren = opt_uint_field(L, -1, "mcounteren", def->mcounteren);
    p->stvec = opt_uint_field(L, -1, "stvec", def->stvec);
    p->sscratch = opt_uint_field(L, -1, "sscratch", def->sscratch);
    p->sepc = opt_uint_field(L, -1, "sepc", def->sepc);
    p->scause = opt_uint_field(L, -1, "scause", def->scause);
    p->stval = opt_uint_field(L, -1, "stval", def->stval);
    p->satp = opt_uint_field(L, -1, "satp", def->satp);
    p->scounteren = opt_uint_field(L, -1, "scounteren", def->scounteren);
    p->ilrsc = opt_uint_field(L, -1, "ilrsc", def->ilrsc);
    p->iflags = opt_uint_field(L, -1, "iflags", def->iflags);
    lua_pop(L, 1);
}

/// \brief Loads HTIF config from Lua.
/// \param L Lua state.
/// \param tabidx Config stack index.
/// \param h HTIF config structure to receive results.
static void check_htif_config(lua_State *L, int tabidx, htif_config &h) {
    if (!opt_table_field(L, tabidx, "htif")) {
        return;
    }
    h.tohost = opt_uint_field(L, -1, "tohost", h.tohost);
    h.fromhost = opt_uint_field(L, -1, "fromhost", h.fromhost);
    h.console_getchar = opt_boolean_field(L, -1, "console_getchar");
    h.yield_progress = opt_boolean_field(L, -1, "yield_progress");
    h.yield_rollup = opt_boolean_field(L, -1, "yield_rollup");
    lua_pop(L, 1);
}

/// \brief Loads C api HTIF config from Lua.
/// \param L Lua state.
/// \param tabidx Config stack index.
/// \param h C api HTIF config structure to receive results
static void check_cm_htif_config(lua_State *L, int tabidx, cm_htif_config *h) {
    if (!opt_table_field(L, tabidx, "htif")) {
        return;
    }
    h->tohost = opt_uint_field(L, -1, "tohost", h->tohost);
    h->fromhost = opt_uint_field(L, -1, "fromhost", h->fromhost);
    h->console_getchar = opt_boolean_field(L, -1, "console_getchar");
    h->yield_progress = opt_boolean_field(L, -1, "yield_progress");
    h->yield_rollup = opt_boolean_field(L, -1, "yield_rollup");
    lua_pop(L, 1);
}

/// \brief Loads CLINT config from Lua to machine_config
/// \param L Lua state
/// \param tabidx Config stack index
/// \param c CLINT config structure to receive results
static void check_clint_config(lua_State *L, int tabidx, clint_config &c) {
    if (!opt_table_field(L, tabidx, "clint")) {
        return;
    }
    c.mtimecmp = opt_uint_field(L, -1, "mtimecmp", c.mtimecmp);
    lua_pop(L, 1);
}

/// \brief Loads C api CLINT config from Lua
/// \param L Lua state
/// \param tabidx Config stack index
/// \param c CLINT config structure to receive results
static void check_cm_clint_config(lua_State *L, int tabidx, cm_clint_config *c) {
    if (!opt_table_field(L, tabidx, "clint")) {
        return;
    }
    c->mtimecmp = opt_uint_field(L, -1, "mtimecmp", c->mtimecmp);
    lua_pop(L, 1);
}

/// \brief Loads DHD config from Lua
/// \param L Lua state
/// \param tabidx Config stack index
/// \param d DHD config structure to receive results
static void check_dhd_config(lua_State *L, int tabidx, dhd_config &d) {
    if (!opt_table_field(L, tabidx, "dhd")) {
        return;
    }
    d.tstart = check_uint_field(L, -1, "tstart");
    d.tlength = check_uint_field(L, -1, "tlength");
    d.dlength = opt_uint_field(L, -1, "dlength", d.dlength);
    d.image_filename = opt_string_field(L, -1, "image_filename");
    d.hlength = opt_uint_field(L, -1, "hlength", d.hlength);
    lua_getfield(L, -1, "h");
    if (lua_istable(L, -1)) {
        for (int i = 1; i <= CM_MACHINE_DHD_H_REG_COUNT; i++) {
            d.h[i-1] = opt_uint_field(L, -1, i, d.h[i-1]);
        }
    } else if (!lua_isnil(L, -1)) {
        luaL_error(L, "invalid dhd.h (expected table)");
    }
    lua_pop(L, 2);
}

/// \brief Loads C api DHD config from Lua
/// \param L Lua state
/// \param tabidx Config stack index
/// \param d C api DHD config structure to receive results
static void check_cm_dhd_config(lua_State *L, int tabidx, cm_dhd_config *d) {
    if (!opt_table_field(L, tabidx, "dhd")) {
        return;
    }
    d->tstart = check_uint_field(L, -1, "tstart");
    d->tlength = check_uint_field(L, -1, "tlength");
    d->dlength = opt_uint_field(L, -1, "dlength", d->dlength);
    d->image_filename = opt_string_field_c(L, -1, "image_filename");
    d->hlength = opt_uint_field(L, -1, "hlength", d->hlength);
    lua_getfield(L, -1, "h");
    if (lua_istable(L, -1)) {
        for (int i = 1; i <= CM_MACHINE_DHD_H_REG_COUNT; i++) {
            d->h[i-1] = opt_uint_field(L, -1, i, d->h[i-1]);
        }
    } else if (!lua_isnil(L, -1)) {
        luaL_error(L, "invalid dhd.h (expected table)");
    }
    lua_pop(L, 2);
}

machine_config clua_check_machine_config(lua_State *L, int tabidx) {
    machine_config c{};
    // Check all parameters from Lua initialization table
    // and copy them to the machine_config object
    check_processor_config(L, tabidx, c.processor);
    check_ram_config(L, tabidx, c.ram);
    check_rom_config(L, tabidx, c.rom);
    check_flash_drive_configs(L, tabidx, c.flash_drive);
    check_htif_config(L, tabidx, c.htif);
    check_clint_config(L, tabidx, c.clint);
    check_dhd_config(L, tabidx, c.dhd);
    return c;
}

cm_machine_config* clua_check_cm_machine_config(lua_State *L, int tabidx) {
    cm_machine_config *c = new cm_machine_config{};
    const cm_machine_config* def_config = cm_new_default_machine_config();
    // Check all parameters from Lua initialization table
    // and copy them to the cm_machine_config object
    // If no lua values do not exist use reasonable default
    check_cm_processor_config(L, tabidx, &c->processor, &def_config->processor);
    check_cm_ram_config(L, tabidx, &c->ram);
    check_cm_rom_config(L, tabidx, &c->rom);
    c->flash_drive_count = check_cm_flash_drive_configs(L, tabidx, &c->flash_drive);
    check_cm_htif_config(L, tabidx, &c->htif);
    check_cm_clint_config(L, tabidx, &c->clint);
    check_cm_dhd_config(L, tabidx, &c->dhd);
    cm_delete_machine_config(def_config);
    return c;
}

/// \brief Loads DHD runtime config from Lua
/// \param L Lua state
/// \param tabidx Runtime config stack index
/// \param d DHD runtime config structure to receive results
static void check_dhd_runtime_config(lua_State *L, int tabidx,
    dhd_runtime_config &d) {
    if (!opt_table_field(L, tabidx, "dhd")) {
        return;
    }
    d.source_address = opt_string_field(L, -1, "source_address");
    lua_pop(L, 1);
}

/// \brief Loads C api DHD runtime config from Lua
/// \param L Lua state
/// \param tabidx Runtime config stack index
/// \param d C api DHD runtime config structure to receive results
static void check_cm_dhd_runtime_config(lua_State *L, int tabidx,
    cm_dhd_runtime_config *d) {
    if (!opt_table_field(L, tabidx, "dhd")) {
        return;
    }
    d->source_address = opt_string_field_c(L, -1, "source_address");
    lua_pop(L, 1);
}

/// \brief Loads concurrency runtime config from Lua.
/// \param L Lua state.
/// \param tabidx Runtime config stack index.
/// \param c concurrency runtime config structure to receive results.
static void check_concurrency_runtime_config(lua_State *L, int tabidx,
    concurrency_config &c) {
    if (!opt_table_field(L, tabidx, "concurrency")) {
        return;
    }
    c.update_merkle_tree = opt_uint_field(L, -1, "update_merkle_tree");
    lua_pop(L, 1);
}

/// \brief Loads C api concurrency runtime config from Lua
/// \param L Lua state
/// \param tabidx Runtime config stack index
/// \param c C api concurrency runtime config structure to receive results
static void check_cm_concurrency_runtime_config(lua_State *L, int tabidx,
                                             cm_concurrency_config *c) {
    if (!opt_table_field(L, tabidx, "concurrency")) {
        return;
    }
    c->update_merkle_tree = opt_uint_field(L, -1, "update_merkle_tree");
    lua_pop(L, 1);
}

machine_runtime_config clua_check_machine_runtime_config(lua_State *L,
    int tabidx) {
    luaL_checktype(L, tabidx, LUA_TTABLE);
    machine_runtime_config r;
    check_dhd_runtime_config(L, tabidx, r.dhd);
    check_concurrency_runtime_config(L, tabidx, r.concurrency);
    return r;
}

cm_machine_runtime_config* clua_check_cm_machine_runtime_config(lua_State *L,
    int tabidx) {
    luaL_checktype(L, tabidx, LUA_TTABLE);
    cm_machine_runtime_config *r = new cm_machine_runtime_config{};
    check_cm_dhd_runtime_config(L, tabidx, &r->dhd);
    check_cm_concurrency_runtime_config(L, tabidx, &r->concurrency);
    return r;
}

machine_runtime_config clua_opt_machine_runtime_config(lua_State *L,
    int tabidx, const machine_runtime_config &r) {
    if (!lua_isnoneornil(L, tabidx)) {
        return clua_check_machine_runtime_config(L, tabidx);
    }
    return r;
}

cm_machine_runtime_config* clua_opt_cm_machine_runtime_config(lua_State *L,
    int tabidx, const cm_machine_runtime_config *r) {
    if (!lua_isnoneornil(L, tabidx)) {
        return clua_check_cm_machine_runtime_config(L, tabidx);
    } else {
        cm_machine_runtime_config *def = new cm_machine_runtime_config{};
        if (r != nullptr) {
            def->concurrency = r->concurrency;
            auto source_address_size = strlen(r->dhd.source_address) + 1;
            def->dhd.source_address = new char[source_address_size];
            strncpy(const_cast<char *>(def->dhd.source_address), r->dhd.source_address, source_address_size);
        }
        return def;
    }
}

} // namespace cartesi
