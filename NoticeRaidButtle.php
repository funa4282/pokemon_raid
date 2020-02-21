<?php
    $slackApiKey = 'YOUR_API_KEY';
    $cmd = '';
    echo exec($cmd, $opt);
    print_r($opt);
    $raid_text = file_get_contents('');
    if ($opt[0] != 0) {
        for ($i = 0; $i < $opt[0]; ++$i) {
            if (strpos($raid_text, $opt[$i + 1]) === false) {
                $text = urlencode($opt[$i + 1].'レイドが始まるよ');
                $url = "https://slack.com/api/chat.postMessage?token=${slackApiKey}&channel=%23pokemon_raid&username=huga-bot&text=${text}&as_user=true";
                file_get_contents($url);
                $img = '';
                $jpg = '.jpg';
                $img = $img.$i.$jpg;
                $params = [
                    'token' => 'YOUR_TOKEN',
                    'channels' => '#',
                    'file' => new CURLFile($img),
                    'filename' => $img,
                    'filetype' => 'jpg',
                    'initial_comment' => 'レイド場所',
                    'title' => 'レイド場所',
                ];

                $ch = curl_init();
                // curl_setopt($ch, CURLOPT_VERBOSE, 1); // デバッグ出力
                curl_setopt_array($ch, [
                    CURLOPT_URL => 'https://slack.com/api/files.upload',
                    CURLOPT_RETURNTRANSFER => true,
                    CURLOPT_POST => true,
                    CURLOPT_POSTFIELDS => $params,
                ]);

                $response = curl_exec($ch);
                curl_close($ch);

                file_put_contents('', $opt[$i + 1], FILE_APPEND);
            }
        }
    }
