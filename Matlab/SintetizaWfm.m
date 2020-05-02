% Din.flag
%
% Din.Nh      Numero de harmonicos para decomposicao/sintese
% Din.Ts      Periodo de execução (seg)
%
% Din.Din.Type    Tipo de forma de onda
%             0: definida por Din.F.Hrm, Din.F.Amp e Din.F.Pha
%             1: quadrada
%             2: triangular
%             3: dente de serra, degrau à esquerda
%             4: dente de serra, degrau à direita
%             5: trapezoidal
%                duração de cada intervalo: 1/8 periodo
%                zero; sobe para um; um; desce para zero; zero; desce para
%                menos um; menos um; sobe para zero
% Din.Freq    Frequencia da onda a sintetizar (Hz)
% Din.DC      Valor DC (valor medio) da onda a sintetizar
% Din.Gain    Ganho a aplicar à forma de onda sintetizada por Fourier

% Devem usar a formula de fourier que usa apenas uma função trigonometrica
% O calculo dos coeficientes deve ser feita apenas quando flag=1

function [y, debug] = SintetizaWfm(Din)

persistent omega; if isempty(omega), omega=0; end
if omega ~= 2*pi*Din.Freq
    omega = 2*pi*Din.Freq;  % Verifica se frequência altera
end

persistent wavetype; if isempty(wavetype),wavetype = 1;end

persistent Ts; if isempty(Ts), Ts=0; end
persistent Nh; if isempty(Nh), Nh=0; end
persistent time; if isempty(time), time = 0;end

persistent r_square; if isempty(r_square),r_square = zeros(1,Nh);end
persistent Beta_square; if isempty(Beta_square),Beta_square = zeros(1,Nh);end
persistent k_square; if isempty(k_square),k_square = zeros(1,Nh);end

persistent r_tri; if isempty(r_tri),r_tri = zeros(1,Nh);end
persistent Beta_tri; if isempty(Beta_tri),Beta_tri = zeros(1,Nh);end
persistent k_tri; if isempty(k_tri),k_tri = zeros(1,Nh);end

persistent r_saw_l; if isempty(r_saw_l),r_saw_l = zeros(1,Nh);end
persistent Beta_saw_l; if isempty(Beta_saw_l),Beta_saw_l = zeros(1,Nh);end
persistent k_saw_l; if isempty(k_saw_l),k_saw_l = zeros(1,Nh);end

persistent r_saw_r; if isempty(r_saw_r),r_saw_r = zeros(1,Nh);end
persistent Beta_saw_r; if isempty(Beta_saw_r),Beta_saw_r = zeros(1,Nh);end
persistent k_saw_r; if isempty(k_saw_r),k_saw_r = zeros(1,Nh);end

persistent r_trap; if isempty(r_trap),r_trap = zeros(1,Nh);end
persistent Beta_trap; if isempty(Beta_trap),Beta_trap = zeros(1,Nh);end
persistent k_trap; if isempty(k_trap),k_trap = zeros(1,Nh);end

%   Configura os harmónicos das ondas para um Nh
if Din.flag == 1
    
    Ts = Din.Ts;
    Nh = Din.Nh;
    time = 0;
    
    ak = zeros(1,Nh);
    bk = zeros(1,Nh);
    
    
    %Quadrada
    for i = 1:Nh
        k_square(i)=2*i-1;
        ak(i)=0;
        bk(i)=-4/(pi*k_square(i));
    end
    r_square=sqrt(ak.^2+bk.^2);
    Beta_square=angle(ak+1i*bk);
    
    %triangular
    ak = zeros(1,Nh);
    bk = zeros(1,Nh);
    for i = 1:Nh
        k_tri(i)=2*i-1;
        ak(i)=0;
        bk(i)=8/(pi^2*i^2)*(-1)^(i-1);
    end
    r_tri=sqrt(ak.^2+bk.^2);
    Beta_tri=angle(ak+1i*bk);
    
   
    %dente de serra, degrau à esquerda
    ak = zeros(1,Nh);
    bk = zeros(1,Nh);
    for i = 1:Nh
        k_saw_l(i)=i;
        ak(i)=0;
        bk(i)=-2*(-1)^i/(pi*i);
    end
    r_saw_l=sqrt(ak.^2+bk.^2);
    Beta_saw_l=angle(ak+1i*bk);
    
    %dente de serra, degrau à direita
    ak = zeros(1,Nh);
    bk = zeros(1,Nh);
    for i = 1:Nh
        k_saw_r(i)=i;
        ak(i)=0;
        bk(i)=2*(-1)^i/(pi*i);
    end
    r_saw_r=sqrt(ak.^2+bk.^2);
    Beta_saw_r=angle(ak+1i*bk);
    
    %trapezoidal
    ak = zeros(1,Nh);
    bk = zeros(1,Nh);
    for i = 1:Nh
        k_trap(i)=i;
        ak(i)=0;
        bk(i)=-(256*sin((pi*i)/8)^3*(2*sin((pi*i)/8)^4 - 3*sin((pi*i)/8)^2 + 1))/(i^2*pi^2);
    end
    r_trap=sqrt(ak.^2+bk.^2);
    Beta_trap=angle(ak+1i*bk);

    y = 0;
	debug=1;
    
    
else
    if wavetype ~= Din.Type
        wavetype = Din.Type;
        time=0;
    end
   	
    switch Din.Type
        case 1
            for i=1:Nh
                y= y + r_square(i)*cos(k_square(i)*omega*time + Beta_square(i));
            end
        case 2
            for i=1:Nh
                y= y + r_tri(i)*cos(k_tri(i)*omega*time + Beta_tri(i));
            end
        case 3
            for i=1:Nh
                y= y + r_saw_l(i)*cos(k_saw_l(i)*omega*time + Beta_saw_l(i));
            end
        case 4
            for i=1:Nh
                y= y + r_saw_r(i)*cos(k_saw_r(i)*omega*time + Beta_saw_r(i));
            end
        case 5
            for i=1:Nh
                y= y + r_trap(i)*cos(k_trap(i)*omega*time + Beta_trap(i));
            end
        otherwise
            k = Din.F.Hrm;
            r = Din.F.Amp;
            Beta = Din.F.Pha;
            for i=1:Nh
                y= y + r(i)*cos(k(i)*omega*time + Beta(i));
            end
    end
    Din.Freq
    time = time + Ts;
    
    y=y*Din.Gain;
    y=y+Din.DC;
    debug=0;
end
