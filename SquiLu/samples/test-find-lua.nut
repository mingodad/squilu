auto str = [==[
<txNomeParlamentar>ABEL MESQUITA JR.</txNomeParlamentar>
<ideCadastro>178957</ideCadastro>
<nuCarteiraParlamentar>1</nuCarteiraParlamentar>
<nuLegislatura>2015</nuLegislatura>
<sgUF>RR</sgUF>
<sgPartido>DEM</sgPartido>
<codLegislatura>55</codLegislatura>
<numSubCota>10</numSubCota>
<txtDescricao>TELEFONIA</txtDescricao>
<numEspecificacaoSubCota>0</numEspecificacaoSubCota>
<txtDescricaoEspecificacao/>
<txtFornecedor>CELULAR FUNCIONAL</txtFornecedor>
<txtCNPJCPF/>
<txtNumero/>
<indTipoDocumento>0</indTipoDocumento>
<datEmissao>2016-01-12T00:00:00</datEmissao>
<vlrDocumento>120.48</vlrDocumento>
<vlrGlosa>0</vlrGlosa>
<vlrLiquido>120.48</vlrLiquido>
<numMes>1</numMes>
<numAno>2016</numAno>
<numParcela>0</numParcela>
<txtPassageiro/>
<txtTrecho/>
<numLote>0</numLote>
<numRessarcimento>0</numRessarcimento>
<nuDeputadoId>3074</nuDeputadoId>
<ideDocumento/>
]==];

auto result_array = [];
auto start_pos = 0;

//auto find_re = "(<[^>]+>)";
//auto find_re = "(<[^>]+>)([^<]*)(</[^>]+>)";
//auto find_re = "<[^>]+>()([^<]*)()</[^>]+>";
auto find_re = "<[^>]+>([^<]*)</[^>]+>";

while(str.find_lua(find_re, result_array, start_pos) >= 0)
{
	print(result_array.join("\t"));
	start_pos = result_array[1]+1;
}
