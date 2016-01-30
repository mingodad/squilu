 local txt = [==[
 UKCompaniesHouseRegisteredNumber|08270000|00000827
EntityCurrentLegalOrRegisteredName|current-period|BRAHMAPOOTRA TEA COMPANY LIMITED
BalanceSheetDate|current-mud|2013-12-31
EntityDormant|current-period|false
StartDateForPeriodCoveredByReport|current-mud|2013-01-01
EndDateForPeriodCoveredByReport|current-mud|2013-12-31
EntityTrading|current-period|true
Debtors|current-mud|197,994
Debtors|previous-mud|197,994
CurrentAssets|current-mud|197,994
CurrentAssets|previous-mud|197,994
CreditorsDueWithinOneYear|current-mud|83,494
CreditorsDueWithinOneYear|previous-mud|83,494
NetCurrentAssetsLiabilities|current-mud|114,500
NetCurrentAssetsLiabilities|previous-mud|114,500
TotalAssetsLessCurrentLiabilities|current-mud|114,500
TotalAssetsLessCurrentLiabilities|previous-mud|114,500
NetAssetsLiabilitiesIncludingPensionAssetLiability|current-mud|114,500
NetAssetsLiabilitiesIncludingPensionAssetLiability|previous-mud|114,500
CalledUpShareCapital|current-mud|114,500
CalledUpShareCapital|previous-mud|114,500
ShareholderFunds|current-mud|114,500
ShareholderFunds|previous-mud|114,500
CompanyEntitledToExemptionUnderSection477CompaniesAct2006|current-period|For the year ending 31 December 2013 the company was entitled to exemption under section 477 of the Companies Act 2006 relating to small companies.
MembersHaveNotRequiredCompanyToObtainAnAudit|current-period|The members have not required the company to obtain an audit in accordance with section 476 of the Companies Act 2006.
DirectorsAcknowledgeTheirResponsibilitiesUnderCompaniesAct|current-period|The directors acknowledge their responsibilities for complying with the requirements of the Act with respect to accounting records and the preparation of accounts.
AccountsAreInAccordanceWithSpecialProvisionsCompaniesActRelatingToSmallCompanies|current-period|These accounts have been prepared in accordance with the provisions applicable to companies subject to the small companies regime.
DateApprovalAccounts|current-mud|27 January 2014
NameEntityOfficer|current-period-director1|V K Mundhra
 ]==];
 
 local zipedTxt = zlib.deflate(txt);
 //zipedTxt = miniz.deflate(txt);
 local unzipedTxt = zlib.inflate(zipedTxt);
 
 print(txt.len(), zipedTxt.len(), unzipedTxt.len(), unzipedTxt == txt);
 
 /*
 zipedTxt = miniz.deflate(txt);
 //zipedTxt = zlib.deflate(txt);
 unzipedTxt = miniz.inflate(zipedTxt);
 
 print(txt.len(), zipedTxt.len(), unzipedTxt.len(), unzipedTxt == txt);
 
 local za = miniz.ZipArchive("/home/mingo/dev/companies-uk/Accounts_Monthly_Data-January2014.zip");
 local file_count = za.get_num_files();
 print(file_count);
 for(local i=0; i < file_count; ++i)
 {
	print(za.get_file_name(i), za.get_file_size(i));
 }
 za.destroy();
 */
 
 local uza = zlib.Unzip("/home/mingo/dev/companies-uk/Accounts_Monthly_Data-January2014.zip");
 local file_count = uza.get_num_files();
 print(__LINE__, file_count);
 print(uza.goto_first_file());
 local x = 0;
 do
 {
 	local finfo = uza.get_file_info();
 	print(finfo.name,finfo.size, finfo.is_dir);
	if(x == 0)
	{
		print(uza.extract_file());
	}
	++x;
} while(uza.goto_next_file());
uza.destroy();
 