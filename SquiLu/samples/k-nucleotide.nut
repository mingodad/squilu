// The Great Computer Language Shootout
// http://shootout.alioth.debian.org/
// contributed by Mike Pall

local function kfrequency(seq, freq, k, frame){
  //local sub = string.sub
  local k1 = k - 1
  for(local i=frame, len= seq.len()-k1; i < len; i += k){
    local c = seq.slice(i, i+k1);
    freq[c]++;
  }
}

local freq_delegate = {
	_get = function(x) {return 0;}
}

local function count(seq, frag){
  local k = frag.len()
  local freq = {};
  freq.setdelegate(freq_delegate)
  for(local frame=0; frame < k; ++frame) kfrequency(seq, freq, k, frame);
  print(freq.get(frag, 0), "\t", frag, "\n")
}

local function frequency(seq, k){
  local freq = {};
  freq.setdelegate(freq_delegate)
  for(local frame=0; frame < k; ++frame) kfrequency(seq, freq, k, frame);
  local sfreq = [], sn = 0
  foreach( c,v in freq) sfreq[sn++] <- c;
  sfreq.sort(function(a, b){
    local fa = freq[a], fb = freq[b]
    return fa == fb && a > b || fa > fb
  })
  sum = seq.len()-k+1
  foreach(c in sfreq){
    print(format("%s %0.3f\n", c, (freq[c]*100)/sum))
  }
  print("\n")
}

local function readseq(){
  //local sub = string.sub
  for line in io.lines() do
    if sub(line, 1, 1) == ">" and sub(line, 2, 6) == "THREE" then break end
  end
  local lines, ln = {}, 0
  for line in io.lines() do
    local c = sub(line, 1, 1)
    if c == ">" then
      break
    elseif c ~= ";" then
      ln = ln + 1
      lines[ln] = line
    end
  end
  return string.upper(table.concat(lines, "", 1, ln))
}

local seq = readseq()
frequency(seq, 1)
frequency(seq, 2)
count(seq, "GGT")
count(seq, "GGTA")
count(seq, "GGTATT")
count(seq, "GGTATTTTAATT")
count(seq, "GGTATTTTAATTTATAGT")