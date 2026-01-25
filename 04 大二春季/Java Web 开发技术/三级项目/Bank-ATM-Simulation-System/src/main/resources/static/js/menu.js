new Vue({
    el: '#app',
    data: {
        balance: null,
        userName: ""
    },
    created() {
        this.getBalance();
        this.getUserName();
    },
    methods: {
        // 登出
        goToLogout() {
            axios.post('/logout')  // 调用后端的登出接口
                .then(response => {
                    alert('登出成功');
                    localStorage.removeItem('balance');  // 清除前端保存的余额
                    window.history.back();
                })
                .catch(error => {
                    alert('登出失败，请稍后再试');
                });
        },
        getUserName() {
            axios.get('/account/username')
                .then(response => {
                    this.userName = response.data.data;
                })
                .catch(error => {
                    alert('获取用户名失败，请重新登录');
                    window.location.href = '/account/login'; // 跳转到登录页面
                })
        },
        // 获取余额
        getBalance() {
            axios.get('/account/balance')
                .then(response => {
                    this.balance = response.data.data; // 假设返回的数据结构中余额在 data 字段
                })
                .catch(error => {
                    alert('获取余额失败，请重新登录');
                    window.location.href = '/account/login'; // 跳转到登录页面
                });
        },
        // 存款页面跳转
        goToDeposit() {
            window.location.href = '/account/deposit';  // 跳转到存款页面
        },
        // 取款页面跳转
        goToWithdraw() {
            window.location.href = '/account/withdraw';  // 跳转到取款页面
        },
        // 转账页面跳转
        goToTransfer() {
            window.location.href = '/account/transfer';  // 跳转到转账页面
        },
        // 修改功能
        goToChange() {
            window.location.href = '/account/change'; // 跳转回登录页面
        },
        goToTransaction() {
            window.location.href = '/account/transaction';
        },
        // 注销功能
        goToCancel() {
            // 弹出确认框，返回 true 表示确认，false 表示取消
            if (confirm('确定要注销账户吗？')) {
                axios.post('/cancel') // 调用注销接口
                    .then(response => {
                        localStorage.removeItem('balance'); // 清除余额
                        alert('账户已注销');
                        window.location.href = '/login'; // 跳转到登录页面
                    })
                    .catch(error => {
                        alert('注销失败，请稍后再试');
                    });
            } else {
                // 用户取消注销操作，什么都不做
                console.log('用户取消注销操作');
            }
        }
    }
});