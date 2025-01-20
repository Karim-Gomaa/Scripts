rule xWmDDA 
{
    meta:
        description = "Detects Xworm Keylogger"
        author = "Karim Gomaa"
        date = "2024-11-3"
	version = "1.0"


    strings:
        $mz_magic = { 4D 5A }          // "MZ" header at the beginning of the file
        $pe_magic = { 50 45 00 00 }       //this is a regular expression
        $s0 = "xWmDDA" ascii wide
        $s1 = "RijndaelManaged" ascii wide
        $s2 = "xWmDDA.exe" ascii wide
        $s3 = "ExecutionPolicy Bypass Add-MpPreference -ExclusionProcess" ascii wide
        $s4 = "ExecutionPolicy Bypass Add-MpPreference -ExclusionPath" ascii wide
        $s5 = "http://ip-api.com/line/?fields=hosting" ascii wide

    condition:
        $mz_magic at 0 and $pe_magic at (uint32(0x3C)) and  2 of ($s*)
}