local str = "AÁÀÂÄEÉÈÊËIÍÌÎÏOÓÒÔÖUÚÙÛÜaáàâäeéèêëiíìîïoóòôöuúùûüNÑnñÇç";
print(str.len(), str.sl_len(), str);
local tmp = str.sl_lower();
print(tmp.len(), tmp.sl_len(), tmp);
print(str.sl_upper());
print(str.sl_deaccent());
print(str.sl_lower_deaccent());

local str1 = "AÁÀÂÄEÉÈÊËIÍÌÎÏOÓÒÔÖUÚÙÛÜÑÇ";
local str2 = "aáàâäeéèêëiíìîïoóòôöuúùûüñç";
print(str1.len(), str1.sl_len(), str2.len(), str2.sl_len());
print("icmp", str1.sl_icmp(str2));
print("sl_icmp_noaccents", str1.sl_icmp_noaccents(str2));
print("sl_cmp_noaccents", str1.sl_cmp_noaccents(str2));

print("sl_like_cmp", str1.sl_like_cmp(str2));
print("sl_like_cmp_noaccents", str1.sl_like_cmp_noaccents(str2));

print("sl_like_cmp", str1.sl_like_cmp("A%"));
print("sl_like_cmp", "A%".sl_like_cmp(str1));
print("sl_like_cmp_noaccents", str1.sl_like_cmp_noaccents("a%"));
print("sl_like_cmp_noaccents", "a%".sl_like_cmp_noaccents(str1));
