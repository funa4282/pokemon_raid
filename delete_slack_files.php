<?php
    $token = 'YOUR_TOKEN';
    $count = 100;
    // このスクリプトはファイル一覧で取得出来たファイルをすべて削除します。
    // 特定のchannelやファイル種別で絞り込むにはオプションを追加してください。
    // 以下のAPIリファレンスを参照願います。
    // https://api.slack.com/methods/files.list
    // 例：channel IDのファイルを取得
    // $op ='token=' . $token . "&count=$count" . "&channel=ABCDEF";
    $op = 'token='.$token."&count=$count".'&channel=CMKS8TUJG';
    $curl = curl_init();
    curl_setopt($curl, CURLOPT_URL, 'https://slack.com/api/files.list?'.$op);
    curl_setopt($curl, CURLOPT_CUSTOMREQUEST, 'GET');
    curl_setopt($curl, CURLOPT_SSL_VERIFYPEER, false);
    curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
    $response = curl_exec($curl);
    $result = json_decode($response, true);
    $n = $result['paging']['total'];
    curl_close($curl);
    $files = $result['files'];

    // https://slack.com/api/files.list　で取得したファイルを削除します。
    $json_data = array('file' => 'XXX');
    $i = 0;
    foreach ($files as $file) {
        echo $file['id'].':'.$file['name'];
        $json_data['file'] = $file['id'];
        $data = json_encode($json_data);
        $ret = postFromHTTP('https://slack.com/api/files.delete', $data, $token);
        if ($ret['ok']) {
            echo ' Delete!';
        } else {
            echo ' ERROR.';
        }
        echo "\n";
        ++$i;
    }
    echo "$n files found.\n";
    echo "delete $i files.\n";

    function postFromHTTP($url, $data, $token)
    {
        $options = array(
                         CURLOPT_RETURNTRANSFER => true,
                         CURLOPT_FOLLOWLOCATION => true,
                         CURLOPT_AUTOREFERER => true,
                         );
        $authorization = 'Authorization: Bearer '.$token;
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, $url);
        curl_setopt($ch, CURLOPT_HTTPHEADER,
                    array('Content-Type: application/json; charset=utf-8', $authorization));
        curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
        curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, false);
        curl_setopt($ch, CURLOPT_VERBOSE, false);
        curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
        curl_setopt($ch, CURLOPT_POST, true);
        curl_setopt_array($ch, $options);
        $result = curl_exec($ch);
        curl_close($ch);
        $ret = json_decode($result, true);

        return $ret;
    }
