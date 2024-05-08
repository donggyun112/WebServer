// <!DOCTYPE html>
// <html lang="ko">
// <head>
//     <meta charset="UTF-8">
//     <meta name="viewport" content="width=device-width, initial-scale=1.0">
//     <title>파일 업로드 및 삭제</title>
//     <style>
//         body {
//             font-family: Arial, sans-serif;
//             background-image: url("ground2.png");
//             background-color: #f2f2f2;
//             display: flex;
//             justify-content: center;
//             align-items: center;
//             height: 100vh;
//             margin: 0;
//         }
//         .container {
//             background-color: white;
//             padding: 40px;
//             border-radius: 10px;
//             box-shadow: 0 0 20px rgba(0, 0, 0, 0.1);
//             text-align: center;
//             max-width: 600px;
//         }
//         h1 {
//             color: #333;
//             font-size: 36px;
//             margin-bottom: 20px;
//         }
//         form {
//             margin-top: 20px;
//         }
//         input[type="file"] {
//             margin-bottom: 10px;
//         }
//         input[type="submit"] {
//             padding: 12px 24px;
//             background-color: #333;
//             color: white;
//             border: none;
//             border-radius: 5px;
//             cursor: pointer;
//             transition: background-color 0.3s ease;
//         }
//         input[type="submit"]:hover {
//             background-color: #666;
//         }
//         ul {
//             list-style-type: none;
//             padding: 0;
//             margin-top: 20px;
//         }
//         li {
//             margin-bottom: 10px;
//         }
//         a {
//             color: #333;
//             text-decoration: none;
//         }
//         a:hover {
//             text-decoration: underline;
//         }
//         .delete-link {
//             color: red;
//             margin-left: 10px;
//         }
//         .delete-link:hover {
//             text-decoration: underline;
//         }
//         .btn {
//             display: inline-block;
//             padding: 12px 24px;
//             background-color: #333;
//             color: white;
//             text-decoration: none;
//             border-radius: 5px;
//             transition: background-color 0.3s ease;
//             margin-top: 20px;
//         }
//         .btn:hover {
//             background-color: #666;
//         }
//     </style>
// </head>
// <body>
//     <div class="container">
//         <h1>파일 업로드 및 삭제</h1>
        
//         <form action="/cgi-bin/upload.py" method="post" enctype="multipart/form-data">
//             <input type="file" name="file">
//             <input type="submit" value="업로드">
//         </form>
        
//         <h2>업로드된 파일 목록</h2>
//         <ul id="file-list">
//             <li><a href="/files/file1.txt">file1.txt</a> <a href="#" class="delete-link" data-file="file1.txt">[삭제]</a></li>
//             <li><a href="/files/file2.jpg">file2.jpg</a> <a href="#" class="delete-link" data-file="file2.jpg">[삭제]</a></li>
//             <li><a href="/files/file3.pdf">file3.pdf</a> <a href="#" class="delete-link" data-file="file3.pdf">[삭제]</a></li>
//         </ul>
        
//         <a href="features.html" class="btn">뒤로 가기</a>
//     </div>

//     <script>
//         // 삭제 링크에 이벤트 리스너 추가
//         var deleteLinks = document.querySelectorAll(".delete-link");
//         deleteLinks.forEach(function(link) {
//             link.addEventListener("click", function(event) {
//                 event.preventDefault();
//                 var fileName = this.getAttribute("data-file");
//                 deleteFile(fileName, this.parentElement);
//             });
//         });

//         // 파일 삭제 요청 보내고 UI 업데이트
//         function deleteFile(fileName, listItem) {
//             fetch("/cgi-bin/delete.py?file=" + fileName, {
//                 method: "DELETE"
//             })
//             .then(function(response) {
//                 if (response.status === 204) {
//                     // 삭제 성공 시 해당 줄 삭제
//                     listItem.remove();
//                 } else {
//                     console.error("파일 삭제에 실패하였습니다.");
//                 }
//             })
//             .catch(function(error) {
//                 console.error("파일 삭제 중 오류 발생:", error);
//             });
//         }
//     </script>
// </body>
// </html>





// if (HTTP_uri == "/cgi-bin/delete.py") {
    // HTML 파일 href 수정하고 버튼 클릭됐을 때 이동하는 && URL HTTP_uri.compare("fileDirectory") 같은 식으로 처리 계획
    //     std::remove("/Users/junsepar/42/webserv/html/8080/cgi/jackson.txt"); //// SERVER_ROOT + HTTP_uri 이런식으로 해야함
    //     response.setStatusCode(204); // 204 반환하면 신경쓸거 없이 그 페이지에 남아있음 UI변경은 알아보면 될거같음
    //     return response;
    // } 위치 잡아줘야함