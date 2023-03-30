print('The NEW event is: ', event)

if event == 'BG' then
    PlaySound('Background')
elseif event == 'RollDice' then
    PlaySound('RollDice')
elseif event == 'Walk' then
	PlaySound('Walk')
elseif event == 'Run' then
	PlaySound('Run')
elseif event == 'Punch' then
	PlaySound('Punch')
elseif event == 'Hurt01' then
	PlaySound('Hurt01')
elseif event == 'Hurt02' then
	PlaySound('Hurt02')
elseif event == 'Death' then
	PlaySound('Death')
end