// theme.js

// 쿠키 설정 함수
function setCookie(name, value, days) {
    let expires = "";
    if (days) {
        let date = new Date();
        date.setTime(date.getTime() + (days * 24 * 60 * 60 * 1000));
        expires = "; expires=" + date.toUTCString();
    }
    document.cookie = name + "=" + (value || "") + expires + "; path=/";
}

// 쿠키 읽기 함수
function getCookie(name) {
    let nameEQ = name + "=";
    let ca = document.cookie.split(';');
    for (let i = 0; i < ca.length; i++) {
        let c = ca[i];
        while (c.charAt(0) == ' ') c = c.substring(1, c.length);
        if (c.indexOf(nameEQ) == 0) return c.substring(nameEQ.length, c.length);
    }
    return null;
}

// 다크 모드 토글 함수
function toggleDarkMode() {
    const body = document.body;
    const button = document.querySelector('.toggle-button');
    body.classList.toggle('dark-mode');
    button.classList.toggle('dark-mode');
    // Save the current mode to cookie
    if (body.classList.contains('dark-mode')) {
        setCookie('theme', 'dark', 7); // 7일 동안 유효
    } else {
        setCookie('theme', 'light', 7);
    }
}

// 페이지 로드 시 쿠키 확인 및 테마 설정
function applyThemeFromCookie() {
    const savedTheme = getCookie('theme');
    if (savedTheme === 'dark') {
        document.body.classList.add('dark-mode');
        const button = document.querySelector('.toggle-button');
        if (button) {
            button.classList.add('dark-mode');
        }
    }
}

// 테마 초기화 및 버튼 이벤트 등록
window.onload = function() {
    applyThemeFromCookie();
    const button = document.querySelector('.toggle-button');
    if (button) {
        button.onclick = toggleDarkMode;
    }
}
