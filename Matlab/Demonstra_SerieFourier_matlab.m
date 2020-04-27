%
% Demonstra a decomposição de sinais periódicos na série de fourier
%
% a0 = (1/T)*int(x(t), t, [0,T]);
% ak = (2/T)*int(x(t)*cos(2*pi*k*t/T), t, [0,T]);
% bk = (2/T)*int(x(t)*sin(2*pi*k*t/T), t, [0,T]);
%
  clear


  % ------ Define o sinal por segmentos ------ 
  % ! piecewise: Não existe em octave !
  %
  % O sinal deve ser definido entre -T/2 e T/2
  %
  op=4;
  if op==1
    % onda quadrada 
    DefSgnl = {...
    {'a', '-T/4',   'T/4'}
    };
  elseif op==2
    % onda triangular 
    DefSgnl = {...
    {' 2*a/T*t + a/2', '-T/2',   '0'}, ...
    {'-2*a/T*t + a/2',    '0', 'T/2'} ...
    };
  elseif op==3
    % onda teste
    DefSgnl = {...
    {'8*t/T+2',  '-T/4', '-T/8'} ...
    {'1',        '-T/8',    '0'} ...
    {'-4*t/T+1',    '0',  'T/3'} ...
    {'-1/3',      'T/3',  'T/2'}
    };
  elseif op==4
    % onda teste
    DefSgnl = {...
    {'0',                                   '-4*T/8', '-T/16-3*T/8'} ...
    {'(t-(-T/16-3*T/8))*(1)/(T/8)',    '-T/16-3*T/8', '-T/16-2*T/8'} ...
    {'1',                              '-T/16-2*T/8', '-T/16-1*T/8'} ...
    {'(t-(-T/16 -T/8))*(-1)/(T/8)+1',  '-T/16-1*T/8',       '-T/16'} ...
    {'0',                                    '-T/16',        'T/16'} ...
    {'(t-(T/16))*(-1)/(T/8)',                 'T/16',  'T/16+1*T/8'} ...
    {'-1',                              'T/16+1*T/8',  'T/16+2*T/8'} ...
    {'(t-(T/16 +2*T/8))*(1)/(T/8)-1',   'T/16+2*T/8',  'T/16+3*T/8'} ...
    {'0',                               'T/16+3*T/8',       '4*T/8'}

    };
  else
    erro
  end

  % ------ Cria função numérica ------
  % Determina x(t) (visualizar conteudo de str_xt)
  str_xt = sprintf('xt=zeros(1,length(t));\n');
  str_xt = sprintf('%stp=t-round(t/T)*T;\n\n', str_xt);
  for i=1:1:length(DefSgnl)
    str_xt = sprintf('%sitmp=(tp>=(%s))&(tp<=(%s));\n', str_xt, ...
        DefSgnl{i}{2}, DefSgnl{i}{3});
    str_xt = sprintf('%sxt(itmp)=%s;\n\n', str_xt, ...
        strrep(DefSgnl{i}{1}, 't', 'tp(itmp)'));
  end
  str_xt = sprintf('%sclear itmp\n', str_xt);

  % ------ Determina a expressão analitica dos coeficientes da série ------
  % Usa Symbolic Package
  syms sk st sT sa

  % ! assume(sk>0) não funciona em octave !
  for i=1:1:length(DefSgnl)
    tmp = DefSgnl{i};
    tmp = strrep(tmp, 't', 'st');
    tmp = strrep(tmp, 'T', 'sT');
    tmp = strrep(tmp, 'a', 'sa');
    s_DefSgnl{i} = tmp;
  end

  % a0
  str='';
  for i=1:1:length(DefSgnl)
    tmp=sprintf('int((%s), st, [%s,%s])', ...
      s_DefSgnl{i}{1}, s_DefSgnl{i}{2}, s_DefSgnl{i}{3});
    str=sprintf('%s + ...\n%s',str, tmp);
  end
  str=sprintf('func=(1/sT)*(%s);',str);
  eval(str);
  s_a0=simplify(func)

  % ak
  str='';
  for i=1:1:length(DefSgnl)
    tmp=sprintf('int((%s)*cos(2*sym(pi)*sk*st/sT), st, [%s,%s])', ...
      s_DefSgnl{i}{1}, s_DefSgnl{i}{2}, s_DefSgnl{i}{3});
    str=sprintf('%s + ...\n%s',str, tmp);
  end
  str=sprintf('func=(2/sT)*(%s);',str);
  eval(str);
  s_ak=simplify(func)

  % bk
  str='';
  for i=1:1:length(DefSgnl)
    tmp=sprintf('int((%s)*sin(2*sym(pi)*sk*st/sT), st, [%s,%s])', ...
      s_DefSgnl{i}{1}, s_DefSgnl{i}{2}, s_DefSgnl{i}{3});
    str=sprintf('%s + ...\n%s',str, tmp);
  end
  str=sprintf('func=(2/sT)*(%s);',str);
  eval(str);
  s_bk=simplify(func)

  % ------ Sintetiza o sinal ------
  % x(t) = a0 + sum_k(a[k]cos(k W t) + b[k]sin(k W t))
  %
  % Com esta formulação poupa-se uma função trigonométrica
  % x(t) = a0 + sum_k(r[k]cos(k W t - beta[k]))

  a = 1;
  T = 1;

  vetkN = [1, 5, 10, 15];
  vetkN = sort(vetkN);

  % ------ Obtém valores numéricos dos coeficientes da série ------
  % subs(func, x, y) : substitui em func x por y
  tmp = subs(s_a0, {sT, sa}, {T, a});
  a0 = double(tmp);

  tmp = subs(s_ak, {sk, sT, sa}, {[1:1:max(vetkN)], T, a});
  ak = double(tmp);

  tmp = subs(s_bk, {sk, sT, sa}, {[1:1:max(vetkN)], T, a});
  bk = double(tmp);

  tmp = ak+1i*bk;
  rk = abs(tmp);
  betak = angle(tmp);

  %--------------------------------
  tmin = -1*T;
  tmax = +1*T;
  tdel = T/1000;
  t=tmin:tdel:tmax;

  % sinais aproximados por fourier
  xtf(1:length(vetkN), 1:length(t))=0;
  W=2*pi/T;
  tmp = zeros(1,length(t)) + a0;
  k0=1;
  for i=1:1:length(vetkN)
    for k=k0:1:vetkN(i)
      tmp = tmp + rk(k)*cos(k*W*t-betak(k));
    end
    xtf(i,:) = tmp;
    k0=vetkN(i)+1;
  end

  % sinal 'real'
  eval(str_xt); % resultado em na variavel xt

  % figura
  figure(1); clf
  set(gcf, 'DefaultAxesFontSize', 14);
  cor=[
    [0, 0.4470, 0.7410]
    [0.8500, 0.3250, 0.0980]
    [0.9290, 0.6940, 0.1250]
    [0.4940, 0.1840, 0.5560]
    [0.4660, 0.6740, 0.1880]
    [0.3010, 0.7450, 0.9330]
    [0.6350, 0.0780, 0.1840]
    [0, 0, 1]
    [1, 0, 0]
  ];
  plot(t, xt, 'k');
  Lgn{1}='x(t)';
  hold on; grid on;
  for i=1:1:length(vetkN)
    plot(t, xtf(i,:), 'color', cor(i,:));
    Lgn{i+1} = sprintf('sN=%d', vetkN(i));
  end
  legend(Lgn);
