#/**
# * @file translation_utils.c
# * @author SpockBotMC
# * @author andersonarc (e.andersonarc@gmail.com)
# * @brief minecraft data to C language converter
# * @version 0.8
# * @date 2020-12-12
# */

from functools import reduce
import minecraft_data
import re
import os

indent = "  "

mcd_type_map = {}
type_pre_definitions = dict(
    char_vector_t="",
    int32_t_vector_t="",
    mcp_type_Slot_vector_t="",
    mcp_type_Tag_vector_t="",
    mcp_type_Item_vector_t="",
    string_t_optional_t="",
    mcp_type_NbtTagCompound_optional_t="",
    int32_t_optional_t="",
    mcp_type_UUID_optional_t="",
    mcp_type_Position_optional_t=""
)
type_definitions = type_pre_definitions.copy()
length_functions = dict()
packet_length_variable = "_this_packet_length"
packet_tmp_variable = "_this_tmp_variable"


def mc_data_name(typename):
    def inner(cls):
        mcd_type_map[typename] = cls
        return cls

    return inner

def remove_prefix(string):
    if string.startswith("mcp_"):
        return string[4:]
    return string

def varnum_length(varnum):
    if(varnum < (1 << 7)):
        return 1
    if(varnum < (1 << 14)):
        return 2
    if(varnum < (1 << 21)):
        return 3
    if(varnum < (1 << 28)):
        return 4
    if(varnum < (1 << 35)):
        return 5
    if(varnum < (1 << 42)):
        return 6
    if(varnum < (1 << 49)):
        return 7
    if(varnum < (1 << 56)):
        return 8
    if(varnum < (1 << 63)):
        return 9
    return 10


# MCD/Protodef has two elements of note, "fields" and "types"
# "Fields" are JSON objects with the following members:
#   * "name" (optional): Name of the field
#   * "anonymous" (optional): Only present if Name is absent, always "True"
#   * "type": A MCD/Protodef "type"
#
# "Types" are either strings or 2-element JSON arrays
#   * String "types" are a string of the type name
#   * JSON Array "types" have the following elements:
#     * 0: String of the type name, same as String "types"
#     * 1: Type data, a JSON object who's members are type-dependent
#
# Of Note: The only MCD "type" that can contain other MCD "fields" is the
# "container" type. Even though MCD "switches" have a type data member called
# "fields" these are not MCD "fields", they are actually MCD "types".
#
# The following extract functions normalize this format

# Param: MCD "type"
# Returns: Field type, Field data
def extract_type(type_field):
    if isinstance(type_field, str):
        return type_field, []
    return type_field[0], type_field[1]


# Param: MCD "field"
# Returns: Field name, Field type, Field data
def extract_field(packet_field):
    name = packet_field.get("name", "")
    field_type, field_data = extract_type(packet_field["type"])
    return name, field_type, field_data


class generic_type:
    typename = ""
    postfix = ""
    
    def __init__(self, name, parent, type_data=None, use_compare=False):
        self.name = name
        self.old_name = None
        self.parent = parent
        self.switched = False

        # This is needed for switches because switches suck ass
        # More descriptively, switches need to be able to change the names of
        # fields when they're constructed in order to avoid name collisions. But
        # when merging later sister fields they need to be able to identify
        # duplicate fields that didn't undergo that transformation.
        #
        # To accomplish this, we repurpose the "name" field strictly as a display
        # name, and do comparisons with the "compare_name".
        self.compare_name = name
        self.use_compare = use_compare

    def length(self, variable):
        return f"*{variable} += sizeof({self.name});",

    def get_type(self):
        return self.typename

    def free(self):
        return ()

    def parameter(self):
        return f"{self.typename} {self.name}"

    def declaration(self):
        return f"{self.typename} {self.name};",

    def initialization(self, value):
        return f"{self.typename} {self.name} = {value};",

    def constructor(self):
        return f"this->{self.name} = {self.name};",

    def encoder(self):
        return f"mcp_encode_{self.postfix}(&{self.name}, dest);",

    def decoder(self):
        return f"mcp_decode_{self.postfix}(&{self.name}, src);",

    def dec_initialized(self):
        return (
            f"{self.typename} {self.name};", 
            f"mcp_decode_{self.postfix}(&{self.name}, src);"
        )

    # This comes up enough to write some dedicated functions for it
    # Conglomerate types take one of two approaches to fundamental types:
    # * Set the field name _every_ time prior to decl/enc/dec
    # * Only change it when needed, and then reset it at the end of the op
    # These support the second workflow
    def temp_name(self, temp):
        if self.old_name is None:
            self.old_name = self.name
        self.name = temp

    def reset_name(self):
        if self.old_name is not None:
            self.name = self.old_name

    def __eq__(self, val):
        if not isinstance(val, type(self)):
            return False
        if hasattr(self, "typename") and hasattr(val, "typename"):
            if self.typename != val.typename:
                return False
        if self.use_compare:
            return self.compare_name == val.compare_name
        return self.name == val.name

    def __str__(self):
        return f"{self.typename} {self.name}"


# A "simple" type is one that doesn't need to typedef an additional type in
# order to declare its variable. The only "complex" types are bitfields and
# containers, everything else is considered "simple"
class simple_type(generic_type):
    pass


class numeric_type(simple_type):
    size = 0

    def encoder(self):
        return f"mcp_encode_{self.postfix}({self.name}, dest);",

# These exist because MCD switches use them. I hate MCD switches
@mc_data_name("void")
class void_type(numeric_type):
    typename = "void"

    def parameter(self):
        return f"/* '{self.name}' is a void type */"

    def constructor(self):
        return f"/* '{self.name}' is a void type */"

    def length(self, variable):
        return f"/* '{self.name}' is a void type */",

    def declaration(self):
        return f"/* '{self.name}' is a void type */",

    def encoder(self):
        return f"/* '{self.name}' is a void type */",

    def decoder(self):
        return f"/* '{self.name}' is a void type */",


@mc_data_name("u8")
class num_u8(numeric_type):
    size = 1
    typename = "uint8_t"
    postfix = "byte"

    def decoder(self):
        return f"mcp_decode_{self.postfix}((uint8_t*) &{self.name}, src);",


@mc_data_name("i8")
class num_i8(num_u8):
    typename = "int8_t"


@mc_data_name("bool")
class num_bool(num_u8):
    pass


@mc_data_name("u16")
class num_u16(numeric_type):
    size = 2
    typename = "uint16_t"
    postfix = "be16"


@mc_data_name("i16")
class num_i16(num_u16):
    typename = "int16_t"

    def decoder(self):
        return f"mcp_decode_{self.postfix}((uint16_t*) &{self.name}, src);",


@mc_data_name("u32")
class num_u32(numeric_type):
    size = 4
    typename = "uint32_t"
    postfix = "be32"


@mc_data_name("i32")
class num_i32(num_u32):
    typename = "int32_t"

    def decoder(self):
        return f"mcp_decode_{self.postfix}((uint32_t*) &{self.name}, src);",


@mc_data_name("u64")
class num_u64(numeric_type):
    size = 8
    typename = "uint64_t"
    postfix = "be64"

@mc_data_name("i64")
class num_i64(num_u64):
    typename = "int64_t"

    def decoder(self):
        return f"mcp_decode_{self.postfix}((uint64_t*) &{self.name}, src);",

@mc_data_name("f32")
class num_float(num_u32):
    typename = "float"
    postfix = "bef32"


@mc_data_name("f64")
class num_double(num_u64):
    typename = "double"
    postfix = "bef64"


# Positions and UUIDs are broadly similar to numeric types
# A position is technically a bitfield but we hide that behind a utility func
@mc_data_name("position")
class num_position(num_u64):
    typename = "mcp_type_Position"
    postfix = "type_Position"

    def encoder(self):
        return f"mcp_encode_{self.postfix}(&{self.name}, dest);",

    def length(self, variable):
        return f"mcp_length_type_Position(&{self.name}, {variable});",


@mc_data_name("UUID")
class num_uuid(numeric_type):
    size = 16
    typename = "mcp_type_UUID"
    postfix = "type_UUID"

    def encoder(self):
        return f"mcp_encode_{self.postfix}(&{self.name}, dest);",

@mc_data_name("varint")
class mc_varint(numeric_type):
    # typename = "std::int32_t"
    # All varints are varlongs until this gets fixed
    # https://github.com/PrismarineJS/minecraft-data/issues/119
    typename = "int64_t"
    postfix = "varint"
    
    def length(self, variable):
        return f"*{variable} += mcp_length_varint({self.name});",

    def decoder(self):
        return f"{self.name} = mcp_decode_{self.postfix}(src);",


@mc_data_name("varlong")
class mc_varlong(numeric_type):
    typename = "int64_t"
    # Decoding varlongs is the same as decoding varints
    postfix = "varint"
    
    def length(self, variable):
        return f"*{variable} += mcp_length_varlong({self.name});",


@mc_data_name("string")
class mc_string(simple_type):
    typename = "string_t"
    postfix = "string"

    def encoder(self):
        return f"mcp_encode_{self.postfix}({self.name}, dest);",
    
    def length(self, variable):
        return f"mcp_length_string({self.name}, {variable});",
    
    def free(self):
        return f"mcp_free_{self.postfix}(&{self.name});",


@mc_data_name("buffer")
class mc_buffer(simple_type):
    typename = "char_vector_t"
    postfix = "buffer"
    
    def __init__(self, name, parent, type_data, use_compare=False):
        super().__init__(name, parent, type_data, use_compare)
        self.count = mcd_type_map[type_data["countType"]]

    def length(self, variable):
        return (
            *self.count(f'{self.name}.size', self).length(variable),
            f"*{variable} += sizeof(*{self.name}.data) * {self.name}.size;"
        )
    
    def encoder(self):
        return (
            *self.count(f'{self.name}.size', self).encoder(),
            f"mcp_encode_buffer(&{self.name}, dest);",
        )

    def decoder(self):
        return (
            *self.count(f'{self.name}.size', self).decoder(),
            f"mcp_decode_buffer(&{self.name}, src);",
        )


@mc_data_name("restBuffer")
class mc_rest_buffer(simple_type):
    typename = "char_vector_t"
    postfix = "buffer"
    
    def length(self, variable):
        return f"*{variable} += (sizeof(*{self.name}.data) * {self.name}.size);",

    def encoder(self):
        return f"mcp_encode_buffer(&{self.name}, dest);",

    def decoder(self):
        return (
            f"{self.name}.size = src->size - src->index;",
            f"mcp_decode_buffer(&{self.name}, src);",
        )


@mc_data_name("nbt")
class mc_nbt(simple_type):
    typename = "mcp_type_NbtTagCompound"
    
    def length(self, variable):
        return f"/* todo nbt length */",

    def encoder(self):
        return f"mcp_encode_type_NbtTagCompound(&{self.name}, dest);", 

    def decoder(self):
        return f"mcp_decode_type_NbtTagCompound(&{self.name}, src);",


@mc_data_name("optionalNbt")
class mc_optional_nbt(simple_type):
    typename = "mcp_type_NbtTagCompound_optional_t"
    
    def length(self, variable):
        return f"/* todo nbt length */",

    def encoder(self):
        return (
            f"if ({self.name}.has_value) {{",
            f"{indent}mcp_encode_type_NbtTagCompound(&{self.name}.value, dest);",
            f"}} else {{",
            f"{indent}{packet_tmp_variable} = MCP_NBT_TAG_END;",
            f"{indent}mcp_encode_byte({packet_tmp_variable}, dest);",
            "}"
        )

    def decoder(self):
        return (
            f"mcp_decode_byte(&{packet_tmp_variable}, src);"
            f"if ({packet_tmp_variable} == MCP_NBT_TAG_COMPOUND) {{",
            f"{indent}{self.name}.has_value = true;",
            f"{indent}mcp_read_type_NbtTagCompound(&{self.name}.value, src);",
            "}"
        )

@mc_data_name("slot")
class mc_slot(simple_type):
    typename = "mcp_type_Slot"
    postfix = "type_Slot"


@mc_data_name("minecraft_smelting_format")
class mc_smelting(simple_type):
    typename = "mcp_type_Smelting"
    postfix = "type_Smelting"

    def free(self):
        return f"mcp_free_{self.postfix}(&{self.name});",


@mc_data_name("entityMetadata")
class mc_metadata(simple_type):
    typename = "mcp_type_EntityMetadata"
    postfix = "type_EntityMetadata"

    def length(self, variable):
        return f"mcp_length_type_EntityMetadata(&{self.name}, {variable});",


# This is not how topBitSetTerminatedArray works, but the real solution is hard
# and this solution is easy. As long as this type is only found in the Entity
# Equipment packet we're going to stick with this solution
@mc_data_name("topBitSetTerminatedArray")
class mc_entity_equipment(simple_type):
    typename = "mcp_type_EntityEquipment"
    postfix = "type_EntityEquipment"

    def length(self, variable):
        return f"mcp_length_type_EntityEquipment(&{self.name}, {variable});",

@mc_data_name("particleData")
class mc_particle(simple_type):
    typename = "mcp_type_Particle"
    postfix = "type_Particle"

    def __init__(self, name, parent, type_data, use_compare=False):
        super().__init__(name, parent, type_data, use_compare)
        self.id_field = type_data["compareTo"]

    def length(self, variable):
        return f"mcp_length_type_Particle(&{self.name}, {variable});",

    def decoder(self):
        return f"mcp_decode_{self.postfix}(&{self.name}, (mcp_type_ParticleType) this->{self.id_field}, src);",


class vector_type(simple_type):
    element = ""
    element_postfix = ""
    should_free_element = False
    count = mc_varint
    
    def __init__(self, name, parent, type_data, use_compare=False):
        super().__init__(name, parent, type_data, use_compare)
        self.depth = 0
        p = parent
        while not isinstance(p, packet):
            self.depth += 1
            p = p.parent

    def length(self, variable):
        iterator = f"i{self.depth}"
        return (
            *self.count(f"{self.name}.size", self).length(variable),
            f"for (size_t {iterator} = 0; {iterator} < {self.name}.size; {iterator}++) {{",
            f"{indent}mcp_length_{self.element_postfix}(&{self.name}.data[{iterator}], {variable});",
            "}",
        )

    def encoder(self):
        iterator = f"i{self.depth}"
        return (
            *self.count(f"{self.name}.size", self).encoder(),
            f"for (size_t {iterator} = 0; {iterator} < {self.name}.size; {iterator}++) {{",
            f"{indent}mcp_encode_{self.element_postfix}(&{self.name}.data[{iterator}], dest);",
            "}",
        )

    def free(self):
        element_free_code = ()
        if self.should_free_element:
            iterator = f"i{self.depth}"
            element_free_code = (
                f"for (size_t {iterator} = 0; {iterator} < {self.name}.size; {iterator}++) {{",
                f"{indent}mcp_free_{self.element_postfix}(&{self.name}.data[{iterator}]);",
                "}"
            )
        return (
            *element_free_code,
            f"free({self.name}.data);",
        )

    def decoder(self):
        iterator = f"i{self.depth}"
        return (
            *self.count(f"{self.name}.size", self).decoder(),
            f"{self.name}.data = malloc({self.name}.size * sizeof({self.element}));",
            f"for (size_t {iterator} = 0; {iterator} < {self.name}.size; {iterator}++) {{",
            f"{indent}mcp_decode_{self.element_postfix}(&{self.name}.data[{iterator}], src);",
            "}"
        )


@mc_data_name("ingredient")
class mc_ingredient(vector_type):
    element = "mcp_type_Slot"
    element_postfix = "type_Slot"
    should_free_element = False
    typename = "mcp_type_Slot_vector_t"


@mc_data_name("tags")
class mc_tags(vector_type):
    element = "mcp_type_Tag"
    element_postfix = "type_Tag"
    should_free_element = True
    typename = "mcp_type_Tag_vector_t"


@mc_data_name("option")
class mc_option(simple_type):
    
    def __init__(self, name, parent, type_data, use_compare=False):
        super().__init__(name, parent, type_data, use_compare)
        f_type, f_data = extract_type(type_data)
        self.field = mcd_type_map[f_type](f"{name}", self, f_data)

        if isinstance(self.field, simple_type):
            self.typename = f"{self.field.typename}_optional_t"
        else:
            self.typename = f"mcp_type_{self.name}_optional_t"

    def parameter(self):
        if isinstance(self.field, simple_type):
            return super().parameter()
        return f"{self.typename} {self.name}"

    def declaration(self):
        if isinstance(self.field, simple_type):
            type_definitions[self.typename] = [f"mcp_generic_optional({self.field.typename})"]
            return super().declaration()
        self.field.temp_name(f"mcp_type_{self.name}")
        type_definitions[self.typename] = self.field.typedef()
        type_definitions[self.typename].append(f"mcp_generic_optional({self.field.name})")
        self.field.reset_name()
        return [f"{self.typename} {self.name};"]

    def length(self, variable):
        self.field.temp_name(f"{self.name}.value")
        result = (
            f"{variable} += sizeof({self.name}.has_value);", 
            *self.field.length(variable)
        )
        self.field.reset_name()
        return result

    def free(self):
        self.field.temp_name(f"{self.name}.value")
        element_free_code = self.field.free()
        self.field.reset_name()
        if len(element_free_code) == 0:
            return ()
        else:
            return (
                f"if ({self.name}.has_value) {{",
                *(indent + line for line in element_free_code),
                "}"
            )

    def encoder(self):
        self.field.temp_name(f"{self.name}.value")
        result = (
            f"mcp_encode_byte({self.name}.has_value, dest);",
            f"if ({self.name}.has_value) {{",
            *(indent + line for line in self.field.encoder()),
            "}"
        )
        self.field.reset_name()
        return result

    def decoder(self):
        ret = [
            f"mcp_decode_byte((uint8_t*) &{packet_tmp_variable}, src);",
            f"if ({packet_tmp_variable} == true) {{"
        ]
        if isinstance(self.field, numeric_type) or type(self.field) in (mc_string,
                                                                        mc_buffer, mc_rest_buffer):
            self.field.temp_name(self.name)
        ret.append(f"{indent}{self.name}.has_value = true;")
        self.field.temp_name(f"{self.name}.value")
        ret.extend(indent + line for line in self.field.decoder())
        self.field.reset_name()
        ret.append("}")
        return ret


class complex_type(generic_type):
    def length(self, variable):
        ret = []
        for field in self.fields:
            ret.extend(field.length(variable))
        return ret

    def free(self):
        ret = []
        for field in self.fields:
            ret.extend(field.free())
        return ret  

    def format_type(self, name, postfix):
        if postfix == 0:
            return f"mcp_type_{name}"
        return f"mcp_type_{name}{postfix}"

    def get_type(self):
        return self.format_type(self.name, self.get_postfix())

    def parameter(self):
        if self.name:
            return f"{self.get_type()} {self.name}"
        parameters = ""
        for field in self.fields:
            parameter = field.parameter()
            if len(parameter) != 0:
                parameters += f"{parameter}, "
        if len(parameters) == 0:
            return parameters
        return parameters[:-2]
    
    def declaration(self):
        if self.name:
            typename = self.get_type()
            self.temp_name(typename)
            if not self.is_defined():
                type_definitions[self.name] = self.typedef()
            self.reset_name()
            return [f"{typename} {self.name};"]
        return [l for f in self.fields for l in f.declaration()]

    def is_defined(self):
        return self.name in type_definitions and type_definitions[self.name] == self.typedef()

    def get_postfix(self):
        count = 0
        name = self.format_type(self.name, count)
        while True:
            if name in type_definitions:
                if type_definitions[name] == self.format_typedef(name):
                    break
                else:
                    count += 1
                    name = self.format_type(self.name, count)
            else:
                break
        if count == 0:
            return ""
        else:
            return str(count)

    def format_typedef(self, name):
        return [
            f"typedef struct {name} {{",
            *(indent + l for f in self.fields for l in f.declaration()),
            f"}} {name};"
        ]

    def typedef(self):
        return self.format_typedef(self.name)


def get_storage(numbits):
    if numbits <= 8:
        return 8
    elif numbits <= 16:
        return 16
    elif numbits <= 32:
        return 32
    else:
        return 64


@mc_data_name("bitfield")
class mc_bitfield(complex_type):
    
    def __init__(self, name, parent, type_data, use_compare=False):
        super().__init__(name, parent, type_data, use_compare)
        lookup_unsigned = {
            8: num_u8,
            16: num_u16,
            32: num_u32,
            64: num_u64
        }
        lookup_signed = {
            8: num_i8,
            16: num_i16,
            32: num_i32,
            64: num_i64
        }
        self.fields = []
        self.extra_data = []
        self.field_sizes = {}

        total = 0
        for idx, field in enumerate(type_data):
            total += field["size"]
            if field["name"] in ("_unused", "unused"):
                continue
            self.field_sizes[field["name"]] = field["size"]
            shift = 0
            for temp in type_data[idx + 1:]:
                shift += temp["size"]
            self.extra_data.append(((1 << field["size"]) - 1, shift, field["size"],
                                    field["signed"]))
            numbits = get_storage(field["size"])
            if field["signed"]:
                self.fields.append(lookup_signed[numbits](field["name"], self))
            else:
                self.fields.append(lookup_unsigned[numbits](field["name"], self))

        self.storage = lookup_unsigned[total](f"{name}_", self)
        self.size = total // 8

    def length(self, variable):
        return f"*{variable} += sizeof({self.storage.typename});",

    def encoder(self):
        ret = [*self.storage.initialization("0")]
        name = f"{self.name}." if self.name else ""
        for idx, field in enumerate(self.fields):
            mask, shift, size, signed = self.extra_data[idx]
            shift_str = f" << {shift}" if shift else ""
            ret.append(f"{self.storage.name} |= ((({self.storage.typename}) {name}{field.name}) & {mask}){shift_str};")
        ret.extend(self.storage.encoder())
        return ret

    def decoder(self):
        ret = [*self.storage.dec_initialized()]
        name = f"{self.name}." if self.name else ""
        for idx, field in enumerate(self.fields):
            mask, shift, size, signed = self.extra_data[idx]
            shift_str = f" >> {shift}" if shift else ""
            ret.append(f"{name}{field.name} = ({self.storage.name}{shift_str}) & {mask};")
            if signed:
                ret.extend((
                    f"if ({name}{field.name} & (1UL << {size - 1})) {{",
                    f"{indent}{name}{field.name} -= 1UL << {size};",
                    "}"
                ))
        return ret


# Whatever you think an MCD "switch" is you're probably wrong
@mc_data_name("switch")
class mc_switch(simple_type):
    length_multiline = True
    
    def __init__(self, name, parent, type_data, use_compare=False):
        super().__init__(name, parent, type_data, use_compare)

        if not (hasattr(self.parent, "name") and self.parent.name):
            self.parent.name = "this->"

        self.compareTo = type_data["compareTo"]
        # Unions are a sum type that differs based on conditions. They are a true
        # "switch". All others are here because MCData is bad and I hate it.
        self.is_union = False

        # A non-union switch consists of one or more consistent types, some or all
        # of which are decoded under different conditions. They act like unions
        # except they leave behind Null switches if they're multi-element.
        #
        # Null switches are fields that got merged into a sister switch, they're
        # carcasses that don't do anything. They're found when MCD encodes a
        # multi-element switch. All the elements except the "lead" element become
        # null switches.
        self.null_switch = False
        # Inverses are default present _except_ for certain conditions, and they're
        # bugged on tons of corner cases that thankfully aren't present in MCD
        # right now.
        self.is_inverse = False
        self.is_str_switch = False

        self.lead_sister = None
        self.fields = []
        self.field_dict = {}

        # Find out if all possible values are the same type, in which case we're
        # not a union. And if all the types are void we're an inverse.
        values = type_data["fields"].values()
        if all(map(lambda x: x == "void", values)):
            self.is_inverse = True
            f_type, f_data = extract_type(type_data["default"])
            self.fields.append(mcd_type_map[f_type](name, self, f_data))
        else:
            values = list(filter(lambda x: x != "void", values))
            self.is_union = not all(x == values[0] for x in values)

        # In the case we're not a union, we need to check for sister switches
        if not self.is_union:
            for field in parent.fields:
                if isinstance(field, mc_switch) and field.compareTo == self.compareTo:
                    self.null_switch = True
                    self.lead_sister = field.lead_sister if field.null_switch else field
                    self.lead_sister.merge(name, type_data["fields"], self.is_inverse,
                                           type_data.get("default", None))
                    return

        self.process_fields(self.name, type_data["fields"])

    def parameter(self):
        if self.null_switch:
            return ()
        parameters = ""
        for field in self.fields:
            parameter = field.parameter()
            if len(parameter) != 0:
                parameters += f"{parameter}, "
        if len(parameters) == 0:
            return parameters
        return parameters[:-2]

    def constructor(self):
        if self.null_switch:
            return ()
        return (l for f in self.fields for l in f.constructor())

    def declaration(self):
        if self.null_switch:
            return ()
        return (l for f in self.fields for l in f.declaration())

    def length(self, variable):
        comp = self.get_compare()
        if self.null_switch:
            return ()
        if self.is_inverse:
            return self.inverse(comp, 2, variable)
        if self.is_str_switch:
            return self.str_switch(comp, 2, variable)
        return self.union_multi(comp, 2, variable)

    def free(self):
        should_free = False
        for field in self.fields:
            if len(field.free()) != 0:
                should_free = True

        if not should_free:
            return ()

        comp = self.get_compare()
        if self.null_switch:
            return "",
        if self.is_inverse:
            return self.inverse(comp, 3)
        if self.is_str_switch:
            return self.str_switch(comp, 3)
        return self.union_multi(comp, 3)

    def code_fields(self, ret, fields, mode, variable=None):
        has_name = hasattr(self.parent, "name") and self.parent.name
        if has_name and self.parent.name.endswith("->"):
            suffix = ""
        else:
            suffix = "."
        for field in fields:
            if has_name:
                field.temp_name(f"{self.parent.name}{suffix}{field.name}")
            if mode == 0:
                ret.extend(f"{indent}{l}" for l in field.encoder())
            elif mode == 1:
                ret.extend(f"{indent}{l}" for l in field.decoder())
            elif mode == 2:
                ret.extend(f"{indent}{l}" for l in field.length(variable))
            elif mode == 3:
                ret.extend(f"{indent}{l}" for l in field.free())
            if has_name:
                field.reset_name()
        return ret

    def inverse(self, comp, mode, variable=None):
        if len(self.field_dict.items()) == 1:
            case, _ = next(iter(self.field_dict.items()))
            ret = [f"if ({comp} != {case}) {{"]
        else:
            return "// Multi-Condition Inverse Not Yet Implemented",
        self.code_fields(ret, self.fields, mode, variable)
        ret.append("}")
        return ret

    # Special case for single condition switches, which are just optionals
    # mascarading as switches
    def optional(self, comp, mode, variable=None):
        case, fields = next(iter(self.field_dict.items()))
        ret = []
        if case == "true":
            ret.append(f"if ({comp}) {{")
        elif case == "false":
            ret.append(f"if (!{comp}) {{")
        elif case.isdigit():
            ret.append(f"if ({comp} == {case}) {{")
        else:
            ret.append(f"if (!strcmp({comp}, {case})) {{")
        self.code_fields(ret, fields, mode, variable)
        ret.append("}")
        return ret

    def str_switch(self, comp, mode, variable=None):
        items = list(self.field_dict.items())
        case, fields = items[0]
        ret = [f"if (!strcmp({comp}, {case})) {{"]
        self.code_fields(ret, fields, mode, variable)
        for case, fields in items[1:]:
            ret.append(f"}} else if (!strcmp({comp}, {case})) {{")
            self.code_fields(ret, fields, mode, variable)
        ret.append("}")
        return ret

    def union_multi(self, comp, mode, variable=None):
        if len(self.field_dict.items()) == 1:
            return self.optional(comp, mode, variable)
        ret = [f"switch ({comp}) {{"]
        for case, fields in self.field_dict.items():
            ret.append(f"{indent}case {case}:")
            tmp = []
            self.code_fields(tmp, fields, mode, variable)
            ret.extend(indent + l for l in tmp)
            ret.append(f"{indent * 2}break;")
        ret.append("}")
        return ret

    # compareTo is a mystical format, its powers unmatched. We don't try to parse
    # it. Instead, we just count the number of hierarchy levels it ascends with
    # ".." and move up the container hierarchy using that. If we hit the packet,
    # we abandon ship and assume the path is absolute.
    def get_compare(self):
        comp = self.compareTo.replace("../", "").replace("..", "").replace("/", ".")
        p = self.parent
        for i in range(self.compareTo.count("..")):
            while not (isinstance(p, complex_type) or isinstance(p, packet)):
                p = p.parent
            if isinstance(p, packet):
                break
            else:
                p = p.parent
        while not (isinstance(p, complex_type) or isinstance(p, packet)):
            p = p.parent
        if not isinstance(p, packet):
            comp = f"{p.name}.{comp}"
        if not comp.startswith("this->"):
            comp = "this->" + comp
        return comp

    def encoder(self):
        comp = self.get_compare()
        if self.null_switch:
            return []
        if self.is_inverse:
            return self.inverse(comp, 0)
        if self.is_str_switch:
            return self.str_switch(comp, 0)
        return self.union_multi(comp, 0)

    def decoder(self):
        comp = self.get_compare()
        if self.null_switch:
            return []
        if self.is_inverse:
            return self.inverse(comp, 1)
        if self.is_str_switch:
            return self.str_switch(comp, 1)
        return self.union_multi(comp, 1)

    def process_fields(self, name, fields):
        for key, field_info in fields.items():
            if not key.isdigit() and key not in ("true", "false"):
                key = f'"{key}"'
                self.is_str_switch = True
            f_type, f_data = extract_type(field_info)
            field = mcd_type_map[f_type](name, self, f_data, True)
            if field not in self.fields:
                if not isinstance(field, void_type):
                    self.fields.append(field)
            if not self.is_inverse and isinstance(field, void_type):
                continue
            if key in self.field_dict:
                self.field_dict[key].append(field)
            else:
                self.field_dict[key] = [field]

            # Look for name collisions and fix them. This is unnecessarily convoluted
            # and ripe for refactoring.
            has_dupes = []
            for field in self.fields:
                if len(list(filter(lambda x: x.compare_name == field.compare_name,
                                   self.fields))) > 1:
                    has_dupes.append(field)
            # If we're an anonymous switch don't bother, we'll break container fields
            if has_dupes and name:
                for key, fields in self.field_dict.items():
                    # Is this fine? Is it guaranteed there will be (at least) one and
                    # only one dupe for each key? I think so?
                    same = next(x for x in fields if x in has_dupes)
                    if self.is_str_switch:
                        tmp = key.replace('"', "").replace(":", "_")
                        same.name = tmp
                        # As a weird consequence of the merging strategy, we can end up in
                        # a situation with two tags that compare the same but have
                        # different names. These tags have the same type, but because
                        # string switch fields have "convenient" names they have different
                        # names. Here we restore these "lost" tags to the field list
                        if all(map(lambda x: x.name != same.name, self.fields)):
                            self.fields.append(same)
                    else:
                        same.name = f"{name}_{key}"

            if not self.is_str_switch:
                self.field_dict = {key: self.field_dict[key] for key in
                                   sorted(self.field_dict)}

    def merge(self, sis_name, sis_fields, is_inverse, default):
        if is_inverse:
            f_type, f_data = extract_type(default)
            self.fields.append(mcd_type_map[f_type](sis_name, self, f_data))
        self.process_fields(sis_name, sis_fields)

    def __eq__(self, value):
        if not super().__eq__(value) or len(self.fields) != len(value.fields):
            return False
        return all([i == j for i, j in zip(self.fields, value.fields)])


# Arrays come in three flavors. Fixed length, length prefixed, and foreign
# field
@mc_data_name("array")
class mc_array(simple_type):
    length_multiline=True

    def __init__(self, name, parent, type_data, use_compare=False):
        super().__init__(name, parent, type_data, use_compare)
        f_type, f_data = extract_type(type_data["type"])
        self.field = mcd_type_map[f_type]("", self, f_data)
        self.depth = 0
        p = parent
        while not isinstance(p, packet):
            self.depth += 1
            p = p.parent
        self.packet = p

        self.is_fixed = False
        self.is_prefixed = False
        self.is_foreign = False
        self.count = None
        if "countType" in type_data:
            self.is_prefixed = True
            self.count = mcd_type_map[type_data["countType"]]("", self, [])
        elif isinstance(type_data["count"], int):
            self.is_fixed = True
            self.count = type_data["count"]
        else:
            self.is_foreign = True
            self.count = type_data["count"]

        if isinstance(self.field, simple_type):
            self.f_type = self.field.typename
        else:
            self.f_type = self.field.name

        self.typename = f"{self.f_type}_vector_t"

    def length(self, variable):
        if self.is_fixed:
            return self.fixed(2, variable)
        if self.is_prefixed:
            return self.prefixed_length(variable)
        return self.foreign_length(variable)

    def get_type(self):
        if isinstance(self.field, simple_type):
            return f"{self.field.typename}_vector_t"
        return f"{self.field.name}_vector_t"

    def parameter(self):
        return f"{self.get_type()} {self.name}"

    def declaration(self):
        ret = []
        if isinstance(self.field, simple_type):
            self.f_type = self.field.typename
            typename = f"{self.f_type}_vector_t"
            type_definitions[typename] = [f"mcp_generic_vector({self.f_type})"]
        else:
            self.field.name = f"mcp_type_{self.packet.packet_name}_{self.name}"
            typename = f"{self.field.name}_vector_t"
            type_definitions[typename] = self.field.typedef()
            self.f_type = self.field.name
            type_definitions[typename].append(f"mcp_generic_vector({self.f_type})")
        if self.is_fixed:
            ret.append(f"{typename} {self.name} /* {self.count} length */;")
        else:
            ret.append(f"{typename} {self.name};")
        return ret

    def fixed(self, mode, variable=None):
        iterator = f"i{self.depth}"
        self.field.temp_name(f"{self.name}.data[{iterator}]")
        ret = [f"for (size_t {iterator} = 0; {iterator} < {self.name}.size; {iterator}++) {{"]
        if mode == 0:
            ret.extend(indent + l for l in self.field.encoder())
        elif mode == 1:
            ret.extend(indent + l for l in self.field.decoder())
        elif mode == 2:
            ret.extend(indent + l for l in self.field.length(variable))
        elif mode == 3:
            ret.extend(indent + l for l in self.field.free())
        ret.append("}")
        self.field.reset_name()
        return ret

    def prefixed_encode(self):
        iterator = f"i{self.depth}"
        self.count.name = f"{self.name}.size"
        self.field.temp_name(f"{self.name}.data[{iterator}]")
        result = (
            *self.count.encoder(),
            f"for (size_t {iterator} = 0; {iterator} < {self.name}.size; {iterator}++) {{",
            *(indent + l for l in self.field.encoder()),
            "}"
        )
        self.field.reset_name()
        return result

    def prefixed_decode(self):
        iterator = f"i{self.depth}"
        self.count.name = f"{self.name}.size"
        self.field.temp_name(f"{self.name}.data[{iterator}]")
        result = (
            *self.count.decoder(),
            f"{self.name}.data = malloc({self.name}.size * sizeof({self.f_type}));",
            f"for (size_t {iterator} = 0; {iterator} < {self.name}.size; {iterator}++) {{",
            *(indent + l for l in self.field.decoder()),
            "}"
        )
        self.field.reset_name()
        return result
    
    def prefixed_length(self, variable):
        iterator = f"i{self.depth}"
        self.count.name = f"{self.name}.size"
        self.field.temp_name(f"{self.name}.data[{iterator}]")
        result = (
            *self.count.length(variable),
            f"for (size_t {iterator} = 0; {iterator} < {self.name}.size; {iterator}++) {{",
            *(indent + l for l in self.field.length(variable)),
            "}"
        )
        self.field.reset_name()
        return result

    # Identical to switches' compareTo
    def get_foreign(self):
        comp = self.count.replace("../", "").replace("..", "").replace("/", ".")
        p = self.parent
        for i in range(self.count.count("..")):
            while not (isinstance(p, complex_type) or isinstance(p, packet)):
                p = p.parent
            if isinstance(p, packet):
                break
            else:
                p = p.parent
        while not (isinstance(p, complex_type) or isinstance(p, packet)):
            p = p.parent
        if not isinstance(p, packet):
            comp = f"{p.name}.{comp}"
        return comp

    def foreign_encode(self):
        iterator = f"i{self.depth}"
        self.field.temp_name(f"{self.name}.data[{iterator}]")
        result = (
            f"for (size_t {iterator} = 0; {iterator} < {self.name}.size; {iterator}++) {{",
            *(indent + l for l in self.field.encoder()),
            "}"
        )
        self.field.reset_name()
        return result

    def foreign_decode(self):
        iterator = f"i{self.depth}"
        self.field.temp_name(f"{self.name}.data[{iterator}]")
        result = (
            f"{self.name}.size = {self.get_foreign()};",
            f"{self.name}.data = malloc({self.name}.size * sizeof({self.f_type}));",
            f"for (size_t {iterator} = 0; {iterator} < {self.name}.size; {iterator}++) {{",
            *(indent + l for l in self.field.decoder()),
            "}"
        )
        self.field.reset_name()
        return result

    def foreign_length(self, variable):
        iterator = f"i{self.depth}"
        self.field.temp_name(f"{self.name}.data[{iterator}]")
        result = (
            f"for (size_t {iterator} = 0; {iterator} < {self.name}.size; {iterator}++) {{",
            *(indent + l for l in self.field.length(variable)),
            "}"
        )
        self.field.reset_name()
        return result

    def free(self):
        if self.is_fixed:
            return self.fixed(3)

        iterator = f"i{self.depth}"

        self.field.temp_name(f"{self.name}.data[{iterator}]")
        field_free_code = self.field.free()
        self.field.reset_name()

        if len(field_free_code) != 0:
            return (
                f"for (size_t {iterator} = 0; {iterator} < {self.name}.size; {iterator}++) {{",
                *(indent + l for l in field_free_code),
                "}",
                f"free({self.name}.data);"
            )
        else:
            return f"free({self.name}.data);",

    def encoder(self):
        if self.is_fixed:
            return self.fixed(0)
        if self.is_prefixed:
            return self.prefixed_encode()
        return self.foreign_encode()

    def decoder(self):
        if self.is_fixed:
            return self.fixed(1)
        if self.is_prefixed:
            return self.prefixed_decode()
        return self.foreign_decode()


# Not a container_type because containers_types sometimes have trivial storage
# requirements. Actual containers always have non-trivial storage, which makes
# them a pure complex type
@mc_data_name("container")
class mc_container(complex_type):
    def __init__(self, name, parent, type_data, use_compare=False):
        super().__init__(name, parent, type_data, use_compare)

        self.fields = []
        for field_info in type_data:
            f_name, f_type, f_data = extract_field(field_info)
            self.fields.append(mcd_type_map[f_type](f_name, self, f_data))

    def length(self, variable):
        ret = []
        if self.name:
            if self.name.endswith("->"):
                suffix = ""
            else:
                suffix = "."
            for field in self.fields:
                field.temp_name(f"{self.name}{suffix}{field.name}")
                ret.extend(field.length(variable))
                field.reset_name()
        else:
            for field in self.fields:
                ret.extend(field.length(variable))
        return ret

    def constructor(self):
        ret = []
        if self.name:
            if self.name.endswith("->"):
                suffix = ""
            else:
                suffix = "."
            for field in self.fields:
                field.temp_name(f"{self.name}{suffix}{field.name}")
                ret.extend(field.constructor())
                field.reset_name()
        else:
            for field in self.fields:
                ret.extend(field.constructor())
        return ret

    def free(self):
        ret = []
        if self.name:
            if self.name.endswith("->"):
                suffix = ""
            else:
                suffix = "."
            for field in self.fields:
                field.temp_name(f"{self.name}{suffix}{field.name}")
                ret.extend(field.free())
                field.reset_name()
        else:
            for field in self.fields:
                ret.extend(field.free())
        return ret

    def encoder(self):
        ret = []
        if self.name:
            if self.name.endswith("->"):
                suffix = ""
            else:
                suffix = "."
            for field in self.fields:
                field.temp_name(f"{self.name}{suffix}{field.name}")
                ret.extend(field.encoder())
                field.reset_name()
        else:
            for field in self.fields:
                ret.extend(field.encoder())
        return ret

    def decoder(self):
        ret = []
        if self.name:
            if self.name.endswith("->"):
                suffix = ""
            else:
                suffix = "."
            for field in self.fields:
                field.temp_name(f"{self.name}{suffix}{field.name}")
                ret.extend(field.decoder())
                field.reset_name()
        else:
            for field in self.fields:
                ret.extend(field.decoder())
        return ret

    def __eq__(self, value):
        if not super().__eq__(value) or len(self.fields) != len(value.fields):
            return False
        return all([i == j for i, j in zip(self.fields, value.fields)])

def get_length(field):
    field.temp_name("this->" + field.name)
    string = field.length(packet_length_variable)
    field.reset_name()
    return string

def get_encoder(field):
    field.temp_name("this->" + field.name)
    string = field.encoder()
    field.reset_name()
    return string

def get_decoder(field):
    field.temp_name("this->" + field.name)
    string = field.decoder()
    field.reset_name()
    return string

def get_free(field):
    field.temp_name("this->" + field.name)
    string = field.free()
    field.reset_name()
    return string

class packet:
    def __init__(self, state, direction, packet_id, packet_id_int, packet_name, data):
        self.state = state
        self.source = "server" if direction == "toClient" else "client"
        self.packet_id = packet_id
        self.packet_id_int = packet_id_int
        self.packet_name = packet_name
        self.data = data
        self.postfix = f"packet_{self.source}_{self.packet_name}"
        self.class_name = f"mcp_packet_{self.source}_{self.packet_name}"

        self.fields = []
        for data_field in data:
            f_name, f_type, f_data = extract_field(data_field)
            self.fields.append(mcd_type_map[f_type](f_name, self, f_data))

    def parameters(self):
        parameters = ""
        for field in self.fields:
            parameter = field.parameter()
            if len(parameter) != 0:
                parameters += f", {parameter}"
        return parameters

    def declaration(self): 
        return [
            f"typedef struct {self.class_name} {{",
            *(indent + l for f in self.fields for l in f.declaration()),
            f"}} {self.class_name};",
            f"void mcp_init_{self.postfix}({self.class_name}* this);",
            f"void mcp_free_{self.postfix}({self.class_name}* this);",
            f"void mcp_create_{self.postfix}({self.class_name}* this{self.parameters()});",
            f"void mcp_decode_{self.postfix}({self.class_name}* this, mcp_buffer_t* src);",
            f"void mcp_encode_{self.postfix}({self.class_name}* this, mcp_buffer_t* src);", 
        ]

    def length(self):
        global packet_length_variable
        previous = packet_length_variable
        packet_length_variable = "length"
        lengths = [*(indent + l for f in self.fields for l in get_length(f))]
        packet_length_variable = previous
        tmp = []
        for line in lengths:
            if packet_tmp_variable in line:
                tmp = [f"{indent}uint8_t {packet_tmp_variable} = 0;"]
        return [
            f"static inline void mcp_length_{self.postfix}({self.class_name}* this, size_t* length) {{",
            f"{indent}*length = {varnum_length(self.packet_id_int)};",
            *tmp,
            *lengths,
            "}"
        ]

    def free(self):
        fields = [*(indent + l for f in self.fields for l in get_free(f))]
        tmp = []
        for line in fields:
            if packet_tmp_variable in line:
                tmp = [f"{indent}uint8_t {packet_tmp_variable} = 0;"]
        return [
            f"void mcp_free_{self.postfix}({self.class_name}* this) {{",
            *tmp,
            *fields,
            "}"
        ]

    def encoder(self):
        fields = [*(indent + l for f in self.fields for l in get_encoder(f))]
        tmp = []
        for line in fields:
            if packet_tmp_variable in line:
                tmp = [f"{indent}uint8_t {packet_tmp_variable} = 0;"]
        return [
            f"void mcp_encode_{self.postfix}({self.class_name}* this, mcp_buffer_t* dest) {{",
            f"{indent}size_t {packet_length_variable};",
            *tmp,
            f"{indent}mcp_length_{self.postfix}(this, &{packet_length_variable});",
            f"{indent}mcp_buffer_allocate(dest, {packet_length_variable});",
            f"{indent}mcp_encode_varint({self.packet_id}, dest);",
            *fields,
            "}"
        ]

    def decoder(self):
        fields = [*(indent + l for f in self.fields for l in get_decoder(f))]
        tmp = []
        for line in fields:
            if packet_tmp_variable in line:
                tmp = [f"{indent}uint8_t {packet_tmp_variable} = 0;"]
        return [
            f"void mcp_decode_{self.postfix}({self.class_name}* this, mcp_buffer_t* src) {{",
            *tmp,
            *fields,
            "}"
        ]

    def constructor(self):
        return [
            f"void mcp_create_{self.postfix}({self.class_name}* this{self.parameters()}) {{",
            *(indent + l for f in self.fields for l in f.constructor()),
            "}"
        ]


mc_states = "handshaking", "status", "login", "play"
mc_directions = "toClient", "toServer"

warning_header = (
    "/**",
    " * @file protocol.h",
    " * @author SpockBotMC",
    " * @author andersonarc (e.andersonarc@gmail.com)",
    " * @brief generated by mcd2packet protocol specification",
    " * @version 0.8",
    " * @date 2020-12-12",
    " */"
)

warning_impl = (
    "/**",
    " * @file protocol.c",
    " * @author SpockBotMC",
    " * @author andersonarc (e.andersonarc@gmail.com)",
    " * @brief generated by mcd2packet protocol specification",
    " * @version 0.8",
    " * @date 2020-12-12",
    " */"
)

warning_particle = (
    "/**",
    " * @file particle.h",
    " * @author SpockBotMC",
    " * @author andersonarc (e.andersonarc@gmail.com)",
    " * @brief generated by mcd2packet minecraft particle types",
    " * @version 0.3",
    " * @date 2020-12-12",
    " */"
)

first_cap_re = re.compile("(.)([A-Z][a-z]+)")
all_cap_re = re.compile("([a-z0-9])([A-Z])")


def to_enum(name, direction, state):
    s1 = first_cap_re.sub(r"\1_\2", name)
    name = all_cap_re.sub(r"\1_\2", s1).upper()
    d = "CL" if direction == "toServer" else "SV"
    st = {"handshaking": "HS", "status": "ST", "login": "LG",
          "play": "PL"}[state]
    return f"MCP_{d}_{st}_{name}"


def to_camel_case(string):
    return string.title().replace("_", "")


def extract_infos_from_listing(listing):
    ret = []
    # Why this seemingly random position inside the full listing? Because mcdata
    # hates you.
    for p_id, p_name in listing["types"]["packet"][1][0]["type"][1]["mappings"].items():
        ret.append((int(p_id, 0), to_camel_case(p_name), f"packet_{p_name}"))
    return ret


def run(version):
    mcd = minecraft_data(version)
    version = version.replace(".", "_")
    proto = mcd.protocol
    header_upper = [
        *warning_header,
        "#ifndef MCP_PROTOCOL_H",
        "#define MCP_PROTOCOL_H",
        "",
        "#include \"mcp/connection.h\"",
        "#include \"mcp/io/buffer.h\"",
        "#include \"mcp/particle.h\"",
        "#include \"mcp/type.h\"",
        "",
        f"#define MCP_MC_VERSION \"{version.replace('_', '.')}\"",
        f"#define MCP_PROTOCOL_VERSION {mcd.version['version']}",
        "",
    ]
    header_lower = [
        "#ifndef NDEBUG",
        f"{indent}extern const char** mcp_protocol_cstrings[MCP_STATE__MAX][MCP_SOURCE__MAX];",
        f"{indent}extern const mcp_packet_id_t mcp_protocol_max_ids[MCP_STATE__MAX][MCP_SOURCE__MAX];",
        "#endif /* NDEBUG */",
        "extern mcp_handler_t** mcp_protocol_handlers[MCP_STATE__MAX][MCP_SOURCE__MAX];",
        ""
    ]
    impl_upper = [
        *warning_impl,
        f"/* MCD version {version.replace('_', '.')} */",
        "#include \"mcp/protocol.h\"",
        "#include \"mcp/handler.h\"",
        "#include \"mcp/codec.h\"",
        "#include <stdlib.h>",
        "#include <string.h>",
        ""
    ]
    impl_lower = []
    particle_header = [
        *warning_particle,
        f"/* MCD version {version.replace('_', '.')} */",
        "#ifndef MCP_PARTICLE_H",
        "#define MCP_PARTICLE_H",
        "",
        "typedef enum mcp_type_ParticleType {"
    ]
    particle_header.extend(f"{indent}MCP_PARTICLE_{x.upper()}," for x in mcd.particles_name)
    particle_header[-1] = particle_header[-1][:-1]
    particle_header += ["} mcp_type_ParticleType;", ""]
    if all(x in mcd.particles_name for x in ("iconcrack", "reddust", "fallingdust", "blockdust")):
        particle_header += [
            "#define MCP_DEFINED_PARTICLE_BLOCK_DUST MCP_PARTICLE_BLOCKDUST",
            "#define MCP_DEFINED_PARTICLE_FALLING_DUST MCP_PARTICLE_FALLINGDUST",
            "#define MCP_DEFINED_PARTICLE_DUST MCP_PARTICLE_REDDUST",
            "#define MCP_DEFINED_PARTICLE_ITEM MCP_PARTICLE_ICONCRACK"
        ]
    elif all(x in mcd.particles_name for x in ("item", "dust", "falling_dust", "block")):
        particle_header += [
            "#define MCP_DEFINED_PARTICLE_BLOCK_DUST MCP_PARTICLE_BLOCK",
            "#define MCP_DEFINED_PARTICLE_FALLING_DUST MCP_PARTICLE_FALLING_DUST",
            "#define MCP_DEFINED_PARTICLE_DUST MCP_PARTICLE_DUST",
            "#define MCP_DEFINED_PARTICLE_ITEM MCP_PARTICLE_ITEM"
        ]
    #elif all(x in mcd.particles_name for x in "")
    particle_header += ["", "#endif /* MCP_PARTICLE_H */", ""]
    packet_enum = {}
    packets = {}
    packet_ids = {}

    for state in mc_states:
        packet_enum[state] = {}
        packets[state] = {}
        for direction in mc_directions:
            packet_enum[state][direction] = []
            packets[state][direction] = []
            packet_info_list = extract_infos_from_listing(proto[state][direction])
            for index, info in enumerate(packet_info_list):
                packet_data = proto[state][direction]["types"][info[2]][1]
                if info[1] != "LegacyServerListPing":
                    packet_id = to_enum(info[1], direction, state)
                    packet_enum[state][direction].append(packet_id)
                else:
                    packet_id = info[0]
                packet_ids[packet_id] = index
                pak = packet(state, direction, packet_id, index, info[1], packet_data)
                if info[1] != "LegacyServerListPing":
                    packets[state][direction].append(pak)
                header_lower += pak.declaration()
                header_lower.append("")

                impl_lower += pak.free()
                impl_lower += pak.constructor()
                impl_lower += pak.length()
                impl_lower += pak.decoder()
                impl_lower += pak.encoder()
                impl_lower.append("")

    for state in mc_states: 
        for direction in mc_directions:
            dr = "server" if direction == "toClient" else "client"
            packet_enum[state][direction].append(f"MCP_{dr.upper()}_{state.upper()}__MAX")
            header_upper.append(f"enum mcp_{dr}_{state}_ids {{")
            header_upper.extend(f"{indent}{l}," for l in packet_enum[state][direction])
            header_upper[-1] = header_upper[-1][:-1]
            header_upper.append("};")
            header_upper.append(f"extern mcp_handler_t* mcp_{dr}_{state}_handlers[MCP_{dr.upper()}_{state.upper()}__MAX];")
            header_upper.append("#ifndef NDEBUG")
            header_upper.append(f"{indent}extern const char* mcp_{dr}_{state}_cstrings[MCP_{dr.upper()}_{state.upper()}__MAX];")
            header_upper.append("#endif /* NDEBUG */")
            header_upper.append("")

    for state in mc_states:
        for direction in mc_directions:
            dr = "server" if direction == "toClient" else "client"
            impl_upper.append("#ifndef NDEBUG")
            impl_upper.append(f"{indent}const char* mcp_{dr}_{state}_cstrings[] = {{")
            for pak in packets[state][direction]:
                impl_upper.append(f"{indent*2}\"{pak.class_name}\",")
            if len(packets[state][direction]) > 1:
                impl_upper[-1] = impl_upper[-1][:-1]
            impl_upper.extend((f"{indent}}};", ""))
            impl_upper.append("#endif /* NDEBUG */")
            impl_upper.append(f"mcp_handler_t* mcp_{dr}_{state}_handlers[MCP_{dr.upper()}_{state.upper()}__MAX] = {{")
            if len(packets[state][direction]) > 1:
                impl_upper.extend([f"{indent}&mcp_handler_Blank,"] * (len(packet_enum[state][direction]) - 1))
                impl_upper[-1] = impl_upper[-1][:-1]
            impl_upper.extend(("};", ""))

    impl_upper += [
        "#ifndef NDEBUG",
        f"{indent}const char** mcp_protocol_cstrings[MCP_STATE__MAX][MCP_SOURCE__MAX] = {{",
        f"{indent*2}{{mcp_client_handshaking_cstrings, mcp_server_handshaking_cstrings}},",
        f"{indent*2}{{mcp_client_status_cstrings, mcp_server_status_cstrings}},",
        f"{indent*2}{{mcp_client_login_cstrings, mcp_server_login_cstrings}},",
        f"{indent*2}{{mcp_client_play_cstrings, mcp_server_play_cstrings}}",
        f"{indent}}};",
        "",
        f"{indent}const mcp_packet_id_t mcp_protocol_max_ids[MCP_STATE__MAX][MCP_SOURCE__MAX] = {{",
        f"{indent*2}{{MCP_CLIENT_HANDSHAKING__MAX, MCP_SERVER_HANDSHAKING__MAX}},",
        f"{indent*2}{{MCP_CLIENT_STATUS__MAX, MCP_SERVER_STATUS__MAX}},",
        f"{indent*2}{{MCP_CLIENT_LOGIN__MAX, MCP_SERVER_LOGIN__MAX}},",
        f"{indent*2}{{MCP_CLIENT_PLAY__MAX, MCP_SERVER_PLAY__MAX}}",
        f"{indent}}};",
        "#endif /* NDEBUG */",
        "",
        "mcp_handler_t** mcp_protocol_handlers[MCP_STATE__MAX][MCP_SOURCE__MAX] = {",
        f"{indent}{{mcp_client_handshaking_handlers, mcp_server_handshaking_handlers}},",
        f"{indent}{{mcp_client_status_handlers, mcp_server_status_handlers}},",
        f"{indent}{{mcp_client_login_handlers, mcp_server_login_handlers}},",
        f"{indent}{{mcp_client_play_handlers, mcp_server_play_handlers}}",
        "};",
        "",
    ]

    for type_pre_definition in type_pre_definitions:
        type_definitions[type_pre_definition] = ""

    for type_definition in type_definitions:
        header_upper.extend(type_definitions[type_definition])
        header_upper.append(" ")

    for length_function in length_functions:
        impl_upper.extend(length_functions[length_function])
        impl_upper.append(" ")

    header = header_upper + header_lower + ["#endif /* MCP_PROTOCOL_H */", ""]
    impl = impl_upper + impl_lower + [""]

    path = ""
    if "MCP_PATH" in os.environ:
        path = os.environ["MCP_PATH"] + "/"

    if not os.path.exists(f"{path}mcp"):
        os.mkdir(f"{path}mcp")
        
    with open(f"{path}mcp/particle.h", "w") as f:
        f.write("\n".join(particle_header))
    with open(f"{path}protocol.c", "w") as f:
        f.write("\n".join(impl))
    with open(f"{path}mcp/protocol.h", "w") as f:
        f.write("\n".join(header))


if __name__ == "__main__":
    run(os.environ["MCP_MC"])
