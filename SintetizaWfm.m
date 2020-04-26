% Din.flag
%
% Din.Nh      Numero de harmonicos para decomposicao/sintese
% Din.Ts      Periodo de execução (seg)
%
% Din.wavetype    Tipo de forma de onda
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

if omega ~= 2*pi*Din.Freq, omega = 2*pi*Din.Freq; end % Verifica se frequência altera

while omega>2*pi        % Para ficar menor que 2*pi
        omega = omega - 2*pi;
end
persistent Ts; if isempty(Ts), Ts=0; end
persistent Nh; if isempty(Nh), Nh=0; end
persistent wavetype; if isempty(wavetype), wavetype=99; end % inicializar numa onda não definida
persistent time; if isempty(time), time = 0;end
if Din.flag==1
    Ts = Din.Ts;
    Nh = Din.Nh;
end

persistent r; if isempty(r),r = zeros(1,Nh);end
persistent Beta; if isempty(Beta),Beta = zeros(1,Nh);end
persistent k; if isempty(k),k = zeros(1,Nh);end

if wavetype ~= Din.Type || (wavetype == 0 && (~isequal(Din.F.Hrm,k) || ~isequal(Din.F.Amp,r) || ~isequal(Din.F.Pha, Beta)))
    wavetype = Din.Type;
    time = 0;
    %custom
    if wavetype==0
        k=Din.F.Hrm;
        r=Din.F.Amp;
        Beta = Din.F.Pha;
    %Quadrada
    elseif wavetype==1
        for i = 1:Nh
            k(i)=i*2-1;
            ak(i)=0;
            bk(i)=4/(pi*k(i));
        end
    %triangular
    elseif wavetype==2
        for i = 1:Nh
            k(i)=2*i-1;
            ak(i)=0;
            bk(i)=8/(pi^2*i^2)*(-1)^(i-1);
        end
    %dente de serra, degrau à esquerda
    elseif wavetype==3
        for i = 1:Nh
            k(i)=i;
            ak(i)=0;
            bk(i)=-2*(-1)^i/(pi*i);
        end
    %dente de serra, degrau à direita
    elseif wavetype==4
        for i = 1:Nh
            k(i)=i;
            ak(i)=0;
            bk(i)=2*(-1)^i/(pi*i);
        end
    %trapezoidal
    elseif wavetype==5
        for i = 0:Nh
            k(i+1)=i;
            ak(i+1)=2/Din.Freq;
            bk(i+1)=0;
        end
    end
    if wavetype ~= 0
        r=sqrt(ak.^2+bk.^2);
        Beta=angle(ak+1i*bk);
    end
end

y=Din.DC;

for i=1:Nh
    y= y + r(i)*cos(k(i)*omega*time + Beta(i));
end
time = time + 40e-3;
y=y*Din.Gain;
debug=1;

end
