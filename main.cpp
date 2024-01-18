#include <cstdio>
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
#include <string>

using u1 = unsigned char;
using u2 = unsigned short;
using u4 = unsigned int;

struct Decoder {
    std::string buff;
    int cursor;

    u1 readU1() {
        return buff[cursor++] & 0xFF;
    }

    u2 readU2() {
        return readU1() << 8 | readU1();
    }

    u4 readU4() {
        return readU2() << 16 | readU2();
    }
};

std::map<u2, std::string> versions = {
    {45, "1.1"},
    {46, "1.2"},
    {47, "1.3"},
    {48, "1.4"},
    {49, "5"},
    {50, "8"},
    {51, "7"},
    {52, "8"},
    {53, "9"},
    {54, "10"},
    {55, "11"},
    {56, "12"},
    {57, "13"},
    {58, "14"},
    {59, "15"},
    {60, "16"},
    {61, "17"},
    {62, "18"},
    {63, "19"},
    {64, "20"},
    {65, "21"},
    {66, "22"},
};

std::map<int, std::string> strings;

int main(int argc, char **argv) {
    if (argc == 1)
        // TODO: printusage
        return 1;

    (void)*argv++; // program

    const char *filepath = *argv++;

    if (!std::filesystem::exists(filepath))
        // TODO: Faiiiiiil
        return 1;

    std::ifstream javaclass(filepath, std::ios::binary);
    std::stringstream source;

    Decoder decoder;
    decoder.cursor = 0;

    if (javaclass.good()) {
        source << javaclass.rdbuf();
        javaclass.close();
        decoder.buff = source.str();
    } else {
        printf("Failed to read file.\n");
        return 1;
    }

    printf("Magic: %x\n", decoder.readU4());
    u2 minor = decoder.readU2();
    printf("Minor: %u (%s)\n", minor, versions[minor].c_str());
    u2 major = decoder.readU2();
    printf("Major: %u (%s)\n", major, versions[major].c_str());
    u2 constantPoolCount = decoder.readU2();
    printf("Constant Pool Count: %u\n", constantPoolCount);

    printf("Constant Pool:\n");
    for (auto i = 0; i < constantPoolCount - 1; i++) {
        printf("\t> ConstantPoolEntry:\n");
        u1 tag = decoder.readU1();
        printf("\t\t> Tag: %u\n", tag);
        switch (tag) {
        case 1: {
            printf("\t\t> Utf8Info:\n");
            printf("\t\t\t> Tag: %u\n", tag);
            u2 len = decoder.readU2();
            printf("\t\t\t> Len: %u\n", len);
            std::string utf8 = "";
            for (auto i = 0; i < len; i++) {
                utf8 += decoder.readU1();
            }
            printf("\t\t\t> Bytes: %s\n", utf8.c_str());
            strings[i + 1] = utf8;
        } break;
        case 7: {
            printf("\t\t> ClassInfo:\n");
            printf("\t\t\t> Tag: %u\n", tag);
            printf("\t\t\t> NameIndex: %u\n", decoder.readU2());
        } break;
        case 8: {
            printf("\t\t> StringInfo:\n");
            printf("\t\t\t> Tag: %u\n", tag);
            printf("\t\t\t> StringIndex: %u\n", decoder.readU2());
        } break;
        case 9:
        case 10:
        case 11: {
            u2 classIndex = decoder.readU2();
            u2 nameAndTypeIndex = decoder.readU2();
            printf("\t\t> RefInfo:\n");
            printf("\t\t\t> Tag: %u\n", tag);
            printf("\t\t\t> ClassIndex: %u\n", classIndex);
            printf("\t\t\t> NameAndTypeIndex: %u\n", nameAndTypeIndex);
        } break;
        case 12: {
            printf("\t\t> NameAndTypeInfo:\n");
            printf("\t\t\t> Tag: %u\n", tag);
            printf("\t\t\t> NameIndex: %u\n", decoder.readU2());
            printf("\t\t\t> DescriptorIndex: %u\n", decoder.readU2());
        } break;
        default: {
            printf("Bad\n");
            return 0;

        } break;
        }
    }

    auto itosb = [](int i) {
        return i == 0 ? "false" : "true";
    };

    u2 accessFlags = decoder.readU2();
    printf("AccessFlags:\n");
    printf("\t> Public: %s\n", itosb(accessFlags & 0x0001));
    printf("\t> Final: %s\n", itosb(accessFlags & 0x0010));
    printf("\t> Super: %s\n", itosb(accessFlags & 0x0020));
    printf("\t> Interface: %s\n", itosb(accessFlags & 0x0200));
    printf("\t> Abstract: %s\n", itosb(accessFlags & 0x0400));
    printf("\t> Synthetic: %s\n", itosb(accessFlags & 0x1000));
    printf("\t> Annotation: %s\n", itosb(accessFlags & 0x2000));
    printf("\t> Enum: %s\n", itosb(accessFlags & 0x4000));

    printf("ThisClass: %u\n", decoder.readU2());
    printf("SuperClass: %u\n", decoder.readU2());

    u2 interfacesCount = decoder.readU2();
    printf("InterfacesCount: %u\n", interfacesCount);
    if (interfacesCount != 0) {
        // TODO
        return 0;
    } else {
        printf("Interfaces: Empty\n");
    }

    u2 fieldsCount = decoder.readU2();
    printf("FieldsCount: %u\n", fieldsCount);
    if (fieldsCount != 0) {
        // TODO
        return 0;
    } else {
        printf("Fields: Empty\n");
    }

    u2 methodsCount = decoder.readU2();
    printf("MethodsCount: %u\n", methodsCount);

    auto printAccessFlags = [&](u2 accessFlags) {
        std::string str = "";
        if (accessFlags & 0x0001)
            str += "Public ";
        if (accessFlags & 0x0002)
            str += "Private ";
        if (accessFlags & 0x0004)
            str += "Protected ";
        if (accessFlags & 0x0008)
            str += "Static ";
        if (accessFlags & 0x0010)
            str += "Final ";
        if (accessFlags & 0x0020)
            str += "Synchronized ";
        if (accessFlags & 0x0040)
            str += "Bridge ";
        if (accessFlags & 0x0080)
            str += "VarArgs ";
        if (accessFlags & 0x0100)
            str += "Native ";
        if (accessFlags & 0x0400)
            str += "Abstract ";
        if (accessFlags & 0x0800)
            str += "Strict ";
        if (accessFlags & 0x1000)
            str += "Synthetic ";
        return str;
    };

    for (auto i = 0; i < methodsCount; i++) {
        printf("\t> MethodEntry:\n");
        u2 accessFlags = decoder.readU2();
        printf("\t\t> AccessFlags: %s\n", printAccessFlags(accessFlags).c_str());
        u2 nameIndex = decoder.readU2();
        printf("\t\t> NameIndex: %u (%s)\n", nameIndex, strings[nameIndex].c_str());
        u2 descIndex = decoder.readU2();
        printf("\t\t> DescriptorIndex: %u (%s)\n", descIndex, strings[descIndex].c_str());
        u2 attrCount = decoder.readU2();
        printf("\t\t> AttributesCount: %u\n", attrCount);
        printf("\t\t> Attributes:\n");
        for (auto j = 0; j < attrCount; j++) {
            printf("\t\t\t> AttributesEntry:\n");
            u2 nameIndex = decoder.readU2();
            printf("\t\t\t\t> NameIndex: %u (%s)\n", nameIndex, strings[nameIndex].c_str());
            u4 attrLen = decoder.readU4();
            printf("\t\t\t\t> AttrLen: %u\n", attrLen);
            if (strings[nameIndex] == "Code") {
                printf("\t\t\t\t> Code attribute:\n");
                u2 maxStack = decoder.readU2();
                printf("\t\t\t\t\t> MaxStack: %u \n", maxStack);
                printf("\t\t\t\t\t> MaxLocals: %u \n", decoder.readU2());
                u4 codeLen = decoder.readU4();
                printf("\t\t\t\t\t> CodeLen: %u \n", codeLen);
                printf("\t\t\t\t\t> Code:\n");
                for (u1 k = 0; k < codeLen;) {
                    u1 opcode = decoder.readU1();
                    switch (opcode) {
                    case 3: {
                        printf("\t\t\t\t\t\t> iconst_0 (%u)\n", opcode);
                        k++;
                    } break;
                    case 4: {
                        printf("\t\t\t\t\t\t> iconst_1 (%u)\n", opcode);
                        k++;
                    } break;
                    case 18: {
                        printf("\t\t\t\t\t\t> ldc %u (%u)\n", decoder.readU1(), opcode);
                        k += 2;
                    } break;
                    case 27: {
                        printf("\t\t\t\t\t\t> iload_1 (%u)\n", opcode);
                        k++;
                    } break;
                    case 28: {
                        printf("\t\t\t\t\t\t> iload_2 (%u)\n", opcode);
                        k++;
                    } break;
                    case 42: {
                        printf("\t\t\t\t\t\t> aload_0 (%u)\n", opcode);
                        k++;
                    } break;
                    case 43: {
                        printf("\t\t\t\t\t\t> aload_1 (%u)\n", opcode);
                        k++;
                    } break;
                    case 60: {
                        printf("\t\t\t\t\t\t> istore_1 (%u)\n", opcode);
                        k++;
                    } break;
                    case 61: {
                        printf("\t\t\t\t\t\t> istore_2 (%u)\n", opcode);
                        k++;
                    } break;
                    case 76: {
                        printf("\t\t\t\t\t\t> astore_1 (%u)\n", opcode);
                        k++;
                    } break;
                    case 89: {
                        printf("\t\t\t\t\t\t> dup (%u)\n", opcode);
                        k++;
                    } break;
                    case 154: {
                        printf("\t\t\t\t\t\t> ifne %u (%u)\n", decoder.readU2(), opcode);
                        k += 3;
                    } break;
                    case 167: {
                        printf("\t\t\t\t\t\t> goto %u (%u)\n", decoder.readU2(), opcode);
                        k += 3;
                    } break;
                    case 177: {
                        printf("\t\t\t\t\t\t> return (%u)\n", opcode);
                        k++;
                    } break;
                    case 178: {
                        printf("\t\t\t\t\t\t> getstatic %u (%u)\n", decoder.readU2(), opcode);
                        k += 3;
                    } break;
                    case 180: {
                        printf("\t\t\t\t\t\t> getfield %u (%u)\n", decoder.readU2(), opcode);
                        k += 3;
                    } break;
                    case 182: {
                        printf("\t\t\t\t\t\t> invokevirtual %u (%u)\n", decoder.readU2(), opcode);
                        k += 3;
                    } break;
                    case 183: {
                        printf("\t\t\t\t\t\t> invokespecial %u (%u)\n", decoder.readU2(), opcode);
                        k += 3;
                    } break;
                    case 187: {
                        printf("\t\t\t\t\t\t> new %u (%u)\n", decoder.readU2(), opcode);
                        k += 3;
                    } break;
                    default: {
                        printf("\t\t\t\t\t\t> Unknown opecode %u\n", opcode);
                        return 0;
                    } break;
                    }
                }
                printf("\t\t\t\t\t> ExceptionTableLength: %u\n", decoder.readU2());
                u2 codeAttrCnt = decoder.readU2();
                printf("\t\t\t\t\t> AttrCount: %u\n", codeAttrCnt);
                printf("\t\t\t\t\t> Attributes:\n");
                for (u2 k = 0; k < codeAttrCnt; k++) {
                    u2 nameIndex = decoder.readU2();
                    printf("\t\t\t\t\t\t> NameIndex: %u (%s)\n", nameIndex, strings[nameIndex].c_str());
                    u4 attrLen = decoder.readU4();
                    printf("\t\t\t\t\t\t> Len: %u\n", attrLen);
                    if (strings[nameIndex] == "LineNumberTable") {
                        printf("\t\t\t\t\t\t> LineNumberTableAttributes:\n");
                        u2 lineNumberLen = decoder.readU2();
                        printf("\t\t\t\t\t\t\t> Len: %u\n", lineNumberLen);
                        for (auto l = 0; l < lineNumberLen; l++) {
                            printf("\t\t\t\t\t\t\t> LineNumber:\n");
                            printf("\t\t\t\t\t\t\t\t> StartPC: %u\n", decoder.readU2());
                            printf("\t\t\t\t\t\t\t\t> LineNumber: %u\n", decoder.readU2());
                        }
                    } else if (strings[nameIndex] == "StackMapTable") {
                        printf("\t\t\t\t\t\t> StackMapTableAttributes:\n");
                        u2 numOfEntries = decoder.readU2();
                        printf("\t\t\t\t\t\t\t> NumberOfEntries: %u\n", numOfEntries);
                        for (auto o = 0; o < numOfEntries; o++) {
                            u1 stackFrameTag = decoder.readU1();
                            printf("\t\t\t\t\t\t\t> StackFrameTag: %u\n", stackFrameTag);
                            if (stackFrameTag >= 252 && stackFrameTag <= 254) {
                                printf("\t\t\t\t\t\t\t\t> FrameType: append_frame\n");
                                printf("\t\t\t\t\t\t\t\t> OffsetDelta: %u\n", decoder.readU2());
                                auto arrLen = stackFrameTag - 251;
                                for (auto l = 0; l < arrLen; l++) {
                                    printf("\t\t\t\t\t\t\t\t> VerificationTypeInfo:\n");
                                    u1 type = decoder.readU1();
                                    switch (type) {
                                    case 1:
                                        printf("\t\t\t\t\t\t\t\t\t> Tag: Integer\n");
                                        break;
                                    case 7: {
                                        printf("\t\t\t\t\t\t\t\t\t> Tag: Object\n");
                                        printf("\t\t\t\t\t\t\t\t\t> ConstantPoolIdx: %u\n", decoder.readU2());
                                    } break;
                                    default:
                                        printf("Bad type %u\n", type);
                                        return 0;
                                    }
                                }
                            } else if(stackFrameTag >= 0 && stackFrameTag <= 63) {
                                printf("\t\t\t\t\t\t\t\t> FrameType: same\n");
                            } else {
                                printf("aldklakd\n");
                                return 0;
                            }
                        }
                    } else {
                        printf("BASDDDDD Not parsing: %s\n", strings[nameIndex].c_str());
                        return 0;
                    }
                }
            } else {
                printf("BASDDDDD Not parsing: %s\n", strings[nameIndex].c_str());
                return 0;
            }
        }
    }

    printf("Strings:\n");
    for (auto str : strings) {
        printf("[%d] %s\n", str.first, str.second.c_str());
    }

    return 0;
}