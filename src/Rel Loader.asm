#To be inserted at 8023e5fc
.set REGION, 'e' # e(u), u(s), j(p), k(r)
.set REVISION, 0 # 0-1,  0-2,  0-1,  0

.set memcpy, 0x80004000 # region free
.if (REGION == 'e') # both revisions have identical dols
    .set DVDConvertPathToEntrynum, 0x8027b910
    .set fileAlloc, 0x8019f7dc
    .set fileAsync, 0x8019fd24
    .set fileFree, 0x8019fa8c
    .set __memAlloc, 0x801a626c
    .set OSFatal, 0x802729b8
    .set OSLink, 0x80274c0c
    .set relWork, 0x80534f98
.elseif ((REGION == 'u') && (REVISION == 0))
    .set DVDConvertPathToEntrynum, 0x80278F74
    .set fileAlloc, 0x8019EA50
    .set fileAsync, 0x8019EF98
    .set fileFree, 0x8019ED00
    .set __memAlloc, 0x801A5634
    .set OSFatal, 0x80270198
    .set OSLink, 0x802723CC
    .set relWork, 0x804f1f90
.elseif ((REGION == 'u') && (REVISION == 1))
    .set DVDConvertPathToEntrynum, 0x802797C0
    .set fileAlloc, 0x8019EAAC
    .set fileAsync, 0x8019EFF4
    .set fileFree, 0x8019ED5C
    .set __memAlloc, 0x801A5690
    .set OSFatal, 0x80270868
    .set OSLink, 0x80272ABC
    .set relWork, 0x804f3818
.elseif ((REGION == 'u') && (REVISION == 2))
    .set DVDConvertPathToEntrynum, 0x80279860
    .set fileAlloc, 0x8019EDC4
    .set fileAsync, 0x8019F30C
    .set fileFree, 0x8019F074
    .set __memAlloc, 0x801A59A8
    .set OSFatal, 0x80270908
    .set OSLink, 0x80272B5C
    .set relWork, 0x804f3998
.elseif ((REGION == 'j') && (REVISION == 0))
    .set DVDConvertPathToEntrynum, 0x80278F24
    .set fileAlloc, 0x8019EA40
    .set fileAsync, 0x8019EF88
    .set fileFree, 0x8019ECF0
    .set __memAlloc, 0x801A5624
    .set OSFatal, 0x80270148
    .set OSLink, 0x8027237C
    .set relWork, 0x804c7290
.elseif ((REGION == 'j') && (REVISION == 1))
    .set DVDConvertPathToEntrynum, 0x80279720
    .set fileAlloc, 0x8019EA88
    .set fileAsync, 0x8019EFD0
    .set fileFree, 0x8019ED38
    .set __memAlloc, 0x801A566C
    .set OSFatal, 0x802707C8
    .set OSLink, 0x80272A1C
    .set relWork, 0x804c8898
.elseif ((REGION == 'k') && (REVISION == 0))
    .set DVDConvertPathToEntrynum, 0x8027F85C
    .set fileAlloc, 0x8019B8B0
    .set fileAsync, 0x8019BDF8
    .set fileFree, 0x8019BB60
    .set __memAlloc, 0x8019EB44
    .set OSFatal, 0x80275114
    .set OSLink, 0x80277328
    .set relWork, 0x8056c928
.else
    .err # Unknown version
.endif

# r31: startdata pointer
# r30: file record pointer then bss pointer
# r29: file record data holder pointer
# r28: rel memory pointer

start:

# push stack
mflr r0
stw r0, 4 (r1)
stwu r1, -24 (r1)
stmw r28, 8 (r1)

# Get pointer to data
bl enddata

startdata:

removeEntrynumCheckInstr:
b tryFileAsync - entrynumCheck

p_DVDConvertPathToEntrynum:
.long DVDConvertPathToEntrynum
p_memcpy:
.long memcpy
p_fileAlloc:
.long fileAlloc
p_fileAsync:
.long fileAsync
p_fileFree:
.long fileFree
p_OSFatal:
.long OSFatal
p_OSLink:
.long OSLink
p_memAlloc:
.long __memAlloc
p_relWork:
.long relWork

relPath:
.string "./mod/mod.rel"

noRelMsg:
.string "ERROR: mod.rel was not found"
noLoadMsg:
.string "ERROR: failed to load mod.rel"
foreground:
.byte 0xff, 0xff, 0xff, 0xff
background:
.byte 0, 0, 0, 0xff

.align 2
enddata:

mflr r31

# Check if the game's rel is loaded and this rel isn't
lwz r3, p_relWork - startdata (r31)
lbz r4, 9 (r3)
cmpwi r4, 0
beq end
lbz r4, 8 (r3)
cmpwi r4, 0
bne end

# Try get entrynum for "./mod/mod.rel"
entrynumCheck:
addi r3, r31, relPath - startdata
lwz r0, p_DVDConvertPathToEntrynum - startdata (r31)
mtlr r0
blrl
cmpwi r3, -1
bne+ haveEntrynum

# Inform user file could not be found
lwz r3, p_relWork - startdata (r31)
li r0, 1
stb r0, 8 (r3)
b end

# Stop this check running again
haveEntrynum:
lwz r0, removeEntrynumCheckInstr - startdata (r31)
stw r0, entrynumCheck - startdata (r31)
addi r3, r31, entrynumCheck - startdata
dcbf 0, r3
icbi 0, r3

# Call fileAsync
tryFileAsync:
addi r3, r31, relPath - startdata
li r4, 0
li r5, 0
lwz r0, p_fileAsync - startdata (r31)
mtlr r0
blrl
cmpwi r3, 0
beq+ end # file not ready yet

# Call fileAlloc
addi r3, r31, relPath - startdata
li r4, 0
li r5, 0
lwz r0, p_fileAlloc - startdata (r31)
mtlr r0
blrl
cmpwi r3, -1
bne+ fileloaded

# Inform user file could not be loaded
addi r3, r31, foreground - startdata
addi r4, r31, background - startdata
addi r5, r31, noLoadMsg - startdata
lwz 0, p_OSFatal - startdata (r31)
mtlr r0
blrl
b end

fileloaded:

# Back up record pointer and get data holder pointer
mr r30, r3
lwz r29, 0xa4 (r3)

# Allocate memory for rel
li r3, 0
lwz r4, 4 (r29)
lwz r0, p_memAlloc - startdata (r31)
mtlr r0
blrl
mr r28, r3

# Copy rel into memory
lwz r4, 0 (r29)
lwz r5, 4 (r29)
lwz r0, p_memcpy - startdata (r31)
mtlr r0
blrl

# Free file
mr r3, r30
lwz r0, p_fileFree - startdata (r31)
mtlr r0
blrl

# Allocate bss - no need to memset to 0 like game's rel loading
# function does, OSLink already does that
li r3, 0
lwz r4, 0x20 (r28)
lwz r0, p_memAlloc - startdata (r31)
mtlr r0
blrl

# Link rel
mr r4, r3
mr r3, r28
lwz r0, p_OSLink - startdata (r31)
mtlr r0
blrl

# Call rel prolog
lwz r0, 0x34 (r28)
mtlr r0
blrl

# Stop this code running again and let the game continue
lwz r3, p_relWork - startdata (r31)
li r0, 1
stb r0, 8 (r3)

end:
# pop stack
lmw r28, 8 (r1)
addi r1, r1, 24
lwz r0, 4 (r1)
mtlr r0
blr