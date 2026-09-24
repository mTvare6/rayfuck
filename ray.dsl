var rdstate
var rdmore
var rdout
var randx
var randy
var randz
var hitx
var hity
var hitz
var hitnx
var hitny
var hitnz
var hitt
var hitmat
var hsfound
var hsox
var hsoy
var hsoz
var hsdx
var hsdy
var hsdz
var hscx
var hscy
var hscz
var hsr
var hsmat
var hstmin
var hstmax
var hwfound
var hwox
var hwoy
var hwoz
var hwdx
var hwdy
var hwdz
var rcox
var rcoy
var rcoz
var rcdx
var rcdy
var rcdz
var rcdepth
var rcoutr
var rcoutg
var rcoutb
var rux
var ruy
var ruz
var rut
var rulensq
var rulen
var rugt
var rule
var ruok
var runot
var hsoffx
var hsoffy
var hsoffz
var hsa
var hsh
var hsc
var hsdisc
var hsroot
var hssqrt
var hsoutx
var hsouty
var hsoutz
var hstmp
var hslow
var hshigh
var hsbad
var hsfront
var hwclose
var rcpx
var rcpy
var rcpz
var rcnx
var rcny
var rcnz
var rcsx
var rcsy
var rcsz
var rcar
var rcag
var rcab
var rcfuzz
var rcrx
var rcry
var rcrz
var rcrlen
var rcproj
var rcdlen
var rcblend
var rctr
var rctg
var rctb
var rcmat
var rctype
var rcpath
var rcactive
var rcscatter
var rctmp
var rcsmallx
var rcsmally
var rcsmallz
var rcsmall
var rcmetal
var rcnot
var rceq
var maspect
var mwidth
var mheight
var msamples
var mdepth
var mviewh
var mvieww
var mstepx
var mstepy
var mpixelx
var mpixely
var mpixelz
var mtargetx
var mtargety
var mtargetz
var msampler
var msampleg
var msampleb
var mpixelr
var mpixelg
var mpixelb
var mx
var my
var msample
var moffx
var moffy
var mtmp
var mcond
var moutw
var mouth
var moutr
var moutg
var moutb
func randomvalue
mul rdstate rdstate 5
add rdstate rdstate 1
ge rdmore rdstate 256
while rdmore
sub rdstate rdstate 256
ge rdmore rdstate 256
end
div rdout rdstate 256
end
func randomunit
set ruok 0
not runot ruok
while runot
call randomvalue
mul rut rdout 2
sub rux rut 1
call randomvalue
mul rut rdout 2
sub ruy rut 1
call randomvalue
mul rut rdout 2
sub ruz rut 1
mul rut rux rux
copy rulensq rut
mul rut ruy ruy
add rulensq rulensq rut
mul rut ruz ruz
add rulensq rulensq rut
gt rugt rulensq 0
le rule rulensq 1
and ruok rugt rule
not runot ruok
end
sqrt rulen rulensq
div randx rux rulen
div randy ruy rulen
div randz ruz rulen
end
func hitsphere
sub hsoffx hscx hsox
sub hsoffy hscy hsoy
sub hsoffz hscz hsoz
mul hstmp hsdx hsdx
copy hsa hstmp
mul hstmp hsdy hsdy
add hsa hsa hstmp
mul hstmp hsdz hsdz
add hsa hsa hstmp
mul hstmp hsdx hsoffx
copy hsh hstmp
mul hstmp hsdy hsoffy
add hsh hsh hstmp
mul hstmp hsdz hsoffz
add hsh hsh hstmp
mul hstmp hsoffx hsoffx
copy hsc hstmp
mul hstmp hsoffy hsoffy
add hsc hsc hstmp
mul hstmp hsoffz hsoffz
add hsc hsc hstmp
mul hstmp hsr hsr
sub hsc hsc hstmp
mul hstmp hsh hsh
mul hsdisc hsa hsc
sub hsdisc hstmp hsdisc
ge hsfound hsdisc 0
if hsfound
sqrt hssqrt hsdisc
sub hsroot hsh hssqrt
div hsroot hsroot hsa
le hslow hsroot hstmin
ge hshigh hsroot hstmax
or hsbad hslow hshigh
if hsbad
add hsroot hsh hssqrt
div hsroot hsroot hsa
le hslow hsroot hstmin
ge hshigh hsroot hstmax
or hsbad hslow hshigh
end
not hsfound hsbad
if hsfound
copy hitt hsroot
mul hstmp hsroot hsdx
add hitx hsox hstmp
mul hstmp hsroot hsdy
add hity hsoy hstmp
mul hstmp hsroot hsdz
add hitz hsoz hstmp
copy hitmat hsmat
sub hsoutx hitx hscx
div hsoutx hsoutx hsr
sub hsouty hity hscy
div hsouty hsouty hsr
sub hsoutz hitz hscz
div hsoutz hsoutz hsr
mul hstmp hsdx hsoutx
copy hsfront hstmp
mul hstmp hsdy hsouty
add hsfront hsfront hstmp
mul hstmp hsdz hsoutz
add hsfront hsfront hstmp
lt hsfront hsfront 0
if hsfront
copy hitnx hsoutx
copy hitny hsouty
copy hitnz hsoutz
else
neg hitnx hsoutx
neg hitny hsouty
neg hitnz hsoutz
end
end
end
end
func hitworld
set hwclose 32767
set hwfound 0
copy hsox hwox
copy hsoy hwoy
copy hsoz hwoz
copy hsdx hwdx
copy hsdy hwdy
copy hsdz hwdz
set hscx 0
set hscy -100.5
set hscz -1
set hsr 100
set hsmat 0
set hstmin 0.001
copy hstmax hwclose
call hitsphere
if hsfound
copy hwclose hitt
set hwfound 1
end
set hscx 0
set hscy 0
set hscz -1.2
set hsr 0.5
set hsmat 1
copy hstmax hwclose
call hitsphere
if hsfound
copy hwclose hitt
set hwfound 1
end
set hscx -1
set hscy 0
set hscz -1
set hsr 0.5
set hsmat 2
copy hstmax hwclose
call hitsphere
if hsfound
copy hwclose hitt
set hwfound 1
end
set hscx 1
set hscy 0
set hscz -1
set hsr 0.5
set hsmat 3
copy hstmax hwclose
call hitsphere
if hsfound
set hwfound 1
end
end
func raycolor
set rcoutr 0
set rcoutg 0
set rcoutb 0
set rctr 1
set rctg 1
set rctb 1
set rcpath 0
lt rcactive rcpath rcdepth
while rcactive
copy hwox rcox
copy hwoy rcoy
copy hwoz rcoz
copy hwdx rcdx
copy hwdy rcdy
copy hwdz rcdz
call hitworld
if hwfound
copy rcpx hitx
copy rcpy hity
copy rcpz hitz
copy rcnx hitnx
copy rcny hitny
copy rcnz hitnz
copy rcmat hitmat
set rcscatter 1
ge rcmetal rcmat 2
not rcnot rcmetal
if rcnot
set rctype 0
eq rceq rcmat 0
if rceq
set rcar 0.8
set rcag 0.8
set rcab 0
else
set rcar 0.1
set rcag 0.2
set rcab 0.5
end
set rcfuzz 0
else
set rctype 1
eq rceq rcmat 2
if rceq
set rcar 0.8
set rcag 0.8
set rcab 0.8
set rcfuzz 0.3
else
set rcar 0.8
set rcag 0.6
set rcab 0.2
set rcfuzz 1
end
end
not rcnot rctype
if rcnot
call randomunit
add rcsx rcnx randx
add rcsy rcny randy
add rcsz rcnz randz
abs rctmp rcsx
lt rcsmallx rctmp 0.000015
abs rctmp rcsy
lt rcsmally rctmp 0.000015
abs rctmp rcsz
lt rcsmallz rctmp 0.000015
and rcsmall rcsmallx rcsmally
and rcsmall rcsmall rcsmallz
if rcsmall
copy rcsx rcnx
copy rcsy rcny
copy rcsz rcnz
end
else
mul rctmp rcdx rcnx
copy rcproj rctmp
mul rctmp rcdy rcny
add rcproj rcproj rctmp
mul rctmp rcdz rcnz
add rcproj rcproj rctmp
mul rctmp rcproj 2
mul rcrx rctmp rcnx
sub rcrx rcdx rcrx
mul rcry rctmp rcny
sub rcry rcdy rcry
mul rcrz rctmp rcnz
sub rcrz rcdz rcrz
mul rctmp rcrx rcrx
copy rcrlen rctmp
mul rctmp rcry rcry
add rcrlen rcrlen rctmp
mul rctmp rcrz rcrz
add rcrlen rcrlen rctmp
sqrt rcrlen rcrlen
div rcrx rcrx rcrlen
div rcry rcry rcrlen
div rcrz rcrz rcrlen
call randomunit
mul rctmp rcfuzz randx
add rcsx rcrx rctmp
mul rctmp rcfuzz randy
add rcsy rcry rctmp
mul rctmp rcfuzz randz
add rcsz rcrz rctmp
mul rctmp rcsx rcnx
copy rcproj rctmp
mul rctmp rcsy rcny
add rcproj rcproj rctmp
mul rctmp rcsz rcnz
add rcproj rcproj rctmp
gt rcscatter rcproj 0
end
if rcscatter
mul rctr rctr rcar
mul rctg rctg rcag
mul rctb rctb rcab
copy rcox rcpx
copy rcoy rcpy
copy rcoz rcpz
copy rcdx rcsx
copy rcdy rcsy
copy rcdz rcsz
add rcpath rcpath 1
lt rcactive rcpath rcdepth
else
set rcactive 0
end
else
mul rctmp rcdx rcdx
copy rcdlen rctmp
mul rctmp rcdy rcdy
add rcdlen rcdlen rctmp
mul rctmp rcdz rcdz
add rcdlen rcdlen rctmp
sqrt rcdlen rcdlen
div rctmp rcdy rcdlen
add rctmp rctmp 1
mul rcblend rctmp 0.5
mul rctmp rcblend 0.5
sub rctmp 1 rctmp
mul rcoutr rctr rctmp
mul rctmp rcblend 0.3
sub rctmp 1 rctmp
mul rcoutg rctg rctmp
copy rcoutb rctb
set rcactive 0
end
end
end
func main
set rdstate 1
set maspect 16
div maspect maspect 9
set mwidth 400
div mheight mwidth maspect
int mheight mheight
set msamples 100
set mdepth 50
set mviewh 2
mul mvieww mviewh mwidth
div mvieww mvieww mheight
div mstepx mvieww mwidth
neg mstepy mviewh
div mstepy mstepy mheight
div mpixelx mvieww 2
neg mpixelx mpixelx
div mtmp mstepx 2
add mpixelx mpixelx mtmp
div mpixely mviewh 2
div mtmp mstepy 2
add mpixely mpixely mtmp
set mpixelz -1
int moutw mwidth
int mouth mheight
text P3
print2 moutw mouth
text 255
set my 0
lt mcond my mheight
while mcond
set mx 0
lt mcond mx mwidth
while mcond
set mpixelr 0
set mpixelg 0
set mpixelb 0
set msample 0
lt mcond msample msamples
while mcond
call randomvalue
sub moffx rdout 0.5
call randomvalue
sub moffy rdout 0.5
add mtmp mx moffx
mul mtmp mtmp mstepx
add mtargetx mpixelx mtmp
add mtmp my moffy
mul mtmp mtmp mstepy
add mtargety mpixely mtmp
copy mtargetz mpixelz
set rcox 0
set rcoy 0
set rcoz 0
copy rcdx mtargetx
copy rcdy mtargety
copy rcdz mtargetz
copy rcdepth mdepth
call raycolor
copy msampler rcoutr
copy msampleg rcoutg
copy msampleb rcoutb
add mpixelr mpixelr msampler
add mpixelg mpixelg msampleg
add mpixelb mpixelb msampleb
add msample msample 1
lt mcond msample msamples
end
div mpixelr mpixelr msamples
sqrt mpixelr mpixelr
div mpixelg mpixelg msamples
sqrt mpixelg mpixelg
div mpixelb mpixelb msamples
sqrt mpixelb mpixelb
lt mcond mpixelr 0
if mcond
set mpixelr 0
end
gt mcond mpixelr 0.999
if mcond
set mpixelr 0.999
end
lt mcond mpixelg 0
if mcond
set mpixelg 0
end
gt mcond mpixelg 0.999
if mcond
set mpixelg 0.999
end
lt mcond mpixelb 0
if mcond
set mpixelb 0
end
gt mcond mpixelb 0.999
if mcond
set mpixelb 0.999
end
mul moutr mpixelr 256
mul moutg mpixelg 256
mul moutb mpixelb 256
int moutr moutr
int moutg moutg
int moutb moutb
print3 moutr moutg moutb
add mx mx 1
lt mcond mx mwidth
end
add my my 1
lt mcond my mheight
end
end
