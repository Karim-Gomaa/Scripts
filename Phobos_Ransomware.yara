rule Phobos_Ransomware
{
	meta:
	    description = "Detects Phobos ransomware malware pattern"
		generated_by = "Karim Gomaa"
		date = "2024-10-28"
		version = "1.0"
		
	
	/*
	0x745468 7709                          ja 745473h
	0x74546a 817DE800001800                cmp dword ptr [ebp - 18h], 180000h
	0x745471 7211                          jb 745484h
	0x745473 FF7510                        push dword ptr [ebp + 10h]
	0x745476 53                            push ebx
	0x745477 FF750C                        push dword ptr [ebp + 0ch]
	0x74547a FF7508                        push dword ptr [ebp + 8]
	0x74547d E811FDFFFF                    call 745193h
	0x745482 EB0F                          jmp 745493h
	0x745484 FF7510                        push dword ptr [ebp + 10h]
	0x745487 53                            push ebx
	0x745488 FF750C                        push dword ptr [ebp + 0ch]
	0x74548b FF7508                        push dword ptr [ebp + 8]
	0x74548e E8B3F8FFFF                    call 744d46h
	 */
	strings:
	    $mz_magic = { 4D 5A }          // "MZ" header at the beginning of the file
        $pe_magic = { 50 45 00 00 }    // "PE\0\0" signature for the PE header
	
		$chunk_1 = {
			81 7D ?? 00 00 18 00
			72 ??
			FF 75 ??
			(8B ?? ?? | 5?)
			FF 75 ??
			(FF 75 ?? | 5? FF 75 ??)
			E8 ?? ?? ?? ??
			EB ??
			FF 75 ??
			(8B ?? ?? | 5?)
			FF 75 ??
			(FF 75 ?? | 5? FF 75 ??)
			E8 ?? ?? ?? ??
		}
	
	condition:
        $mz_magic at 0 and $pe_magic at (uint32(0x3C)) and $chunk_1
}
