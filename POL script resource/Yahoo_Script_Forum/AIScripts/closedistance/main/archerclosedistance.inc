// Guarda o �ltimo lado para qual o NPC estava virado
global lastface;
// Guarda quando foi que o NPC executou a fun��o Procurar camminho, para evitar runaways e
// sobrecarregamento.
global ultimaprocura;
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//
// FUN��O CLOSEDISTANCE  -  Anda normalmente com o NPC at� o alvo, verifica quando o
//                          NPC engalhou e executa a fun��o "Procurecaminho", caso
//                          n�o ache um caminho, o NPC foge do alvo.
//
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
function CloseDistance( opponent , distancia := 20)
   
    local distancia2 := Distance( me, opponent );

    if (!theammo)
        theammo := CreateItemInContainer(me.backpack, ammotype, ammoamount);
    endif
    if ((distancia2 <= distancia)and(distancia2 > 4)and(CheckLineOfSight(me, opponent)))
        return 1;
    elseif(distancia2 > distancia)
        RunToward( opponent );
        RunToward( opponent );
        RunToward( opponent );
    elseif ((distancia2 < distancia) and !(CheckLineOfSight(me, opponent)))
        if (Procurecaminho(opponent))
            return 2;
        endif
    else
        RunLikeHell(opponent, 6);
    endif

endfunction

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//
// FUN��O PROCURECAMINHO  -  Procura o ponto mais perto do npc que tenha LOS para
//                           o NPC e o inimigo ao mesmo tempo.
//
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
function Procurecaminho(inimigo);
// Varredura do eixo X
local destinoX := 0; // Coordenada X de destino
local destinoY := 0; // Coordenada Y de destino
local distanciaatual := 30; //Distancia entre vc e as coordenadas de destino,
                            //usada para saber qual o melhor caminho
    for X:=1 to 24
// Anti RunAways!
sleepms(1);
        if (CheckLOSat(me, me.x+X-12, me.y, me.z) and CheckLOSat(inimigo, me.x+X-12, me.y, me.z))
            if (coordist( me.x , me.y , me.x+X-12 , me.y ) < distanciaatual)
                destinoX := me.x+X-12;
                destinoY := me.y;
                distanciaatual := coordist( me.x , me.y , me.x+X-12 , me.y );
            endif
        else
// Varredura Y
            for Y:=1 to 24
                if (CheckLOSat(me, me.x+X-12, me.y+Y-12, me.z) and CheckLOSat(inimigo, me.x+X-12, me.y+Y-12, me.z))
                    if (coordist( me.x , me.y , me.x+X-12 , me.y+Y-12 ) < distanciaatual)
                        destinoX := me.x+X-12;
                        destinoY := me.y+Y-12;
                        distanciaatual := coordist( me.x , me.y , me.x+X-12 , me.y );
                    endif
                endif
            endfor
        endif
    endfor
//
// Caso tenha achado um caminho
    if (destinoX)
        return CloseDistancetoLocation( destinoX, destinoY, 0);
    endif
// Caso n�o...
    return 1;
endfunction

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//
// FUN��O CLOSEDISTANCETOLOCATION  -  NPC anda at� as coordenadas XY
//
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
function CloseDistancetoLocation( x, y, distancia := 1)
    local engalhou := 0;
    repeat
// 3 RunToaward consecutivos, pois com apenas 1, o NPC engalha mais facilmente.
// Isso foi testado e comprovado, mas o "pq?" ninguem sabe :P
            RunTowardLocation( x, y );
            RunTowardLocation( x, y );
            RunTowardLocation( x, y );
            local face := me.facing;
            if (ABS(lastface - face) = 4)
                lastface := face;
                engalhou := engalhou+1;
            endif
            lastface := face;
    until ((coordist( me.x , me.y , x , y ) <= distancia) or (engalhou > 1));
    if (engalhou > 1)
        return 1;
    endif
    return 0;
endfunction

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//
// FUN��O RUNLIKEHELL  -  Faz o npc correr para longe do oponente.
//
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
function RunLikeHell(opponent, distancia := 25)
    local ev;
	while ( (opponent) and !(opponent.dead or opponent.hidden or opponent.concealed) and 
	    (dist(me,opponent) < distancia) ) 
        if (dist(me,opponent) < 2)
            repeat
	            case (ev.type)
                    SYSEVENT_DAMAGED:
            		    if ( randomint(3)=1 )
                        	Fight( ev.source );
        		        	return;
            		    endif
                endcase
            until (! (ev := os::wait_for_event(1)) );
        else
            sleepms(3);
            Runawayfrom( opponent );
            sleepms(3);
            Runawayfrom( opponent );
            sleepms(3);
            Runawayfrom( opponent );
            local face := me.facing;
            if (ABS(lastface - face) = 4)
            	local ev := array;
            	ev.+type := EVID_PEACEMADE;
    			SendEvent(me, ev);
                return;
            endif
            lastface := face;
        endif
	endwhile
endfunction

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//
// FUN��O AGRUPAR  -  Verifica se deve agrupar por template, graphic ou n�o agrupar.
//
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

global groupby := npccfg[me.npctemplate].groupby;
// Usando integer no groupby para ocupar menos processamento na fun��o 'agrupar'
case (groupby)
    "template": groupby := 1;
    "graphic": groupby := 2;
    default: groupby := 0;
endcase

function Agrupar()
    local grupo := ListMobilesInLineOfSight( me, 15 );
    if (grupo.size() > 6)
        return;
    endif
    case (groupby)
        0: return;
        1: Agruparportemplate(grupo);
        2: Agruparporgrafico(grupo);
    endcase
endfunction

function Agruparporgrafico(byref grupo)
    foreach animal in grupo
        if ((animal.graphic = me.graphic)and(!animal.warmode))
            if (Distance(me, animal) > 10)
                CaminhePara(animal, 5);
                return;
            endif
        endif
    endforeach
endfunction

function Agruparportemplate(byref grupo)
    foreach animal in grupo
        if ((animal.npctemplate = me.npctemplate)and(!animal.warmode))
            if (Distance(me, animal) > 10)
                CaminhePara(animal, 5);
                return;
            endif
        endif
    endforeach
endfunction
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//
// FUN��O CAMINHEPARA  -  O npc caminha para o mobile, caso engalhe, ele para.
//
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

function caminhepara(alvo, distancia := 1)
    local face;
    local runaway := 0;
    repeat
        face := me.facing;
        WalkToward( alvo );
        WalkToward( alvo );
        WalkToward( alvo );
        if (ABS(me.facing - face) = 4)  //NPC trancado
            runaway := runaway+1;
        endif
    until ((Distance(me, alvo) <= distancia) or (runaway > 2) or (Distance(me, alvo) > 20));
endfunction